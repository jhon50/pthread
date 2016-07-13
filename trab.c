#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define BUF_SIZE 4
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

int buffer[BUF_SIZE];
//int consumidos[BUF_SIZE];
//int produzidos[BUF_SIZE];
int indiceDeEscrita=0;
int indiceDeLeitura=0;
int celulasOcupadas=0;
pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_cons=PTHREAD_COND_INITIALIZER;
pthread_cond_t c_prod=PTHREAD_COND_INITIALIZER;

void *producer(void *param);
void *consumer(void *param);
void *inicializaBuffer();
void *imprime(int inicio, int fim);

main (int argc, char *argv[])
{
	inicializaBuffer();
	imprime(0,BUF_SIZE);

	pthread_t tid1, tid2;
	int i;
	if (pthread_create(&tid1,NULL,producer,NULL) != 0) {
		fprintf (stderr, "Unable to create producer thread\n");
		exit (1);
	}
	if (pthread_create(&tid2,NULL,consumer,NULL) != 0) {
		fprintf (stderr, "Unable to create consumer thread\n");
		exit (1);
	}
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
}
void *producer(void *param)
{
	int i;

	while(1) {
		i = (int)(drand48() * 100.0);	
		
		pthread_mutex_lock (&m);
		while (celulasOcupadas == BUF_SIZE){
			printf(RED "||||[ BUFFER CHEIO ]||||\n\n\n" RESET);
			pthread_cond_wait (&c_prod, &m);
		}

		buffer[indiceDeEscrita] = i;
		printf("PRODUZIU: %d\n\n",i );
		indiceDeEscrita = (indiceDeEscrita+1) % BUF_SIZE;
		celulasOcupadas++;
		imprime(0,celulasOcupadas);
		sleep((int)(drand48() * 4.0));
		pthread_mutex_unlock (&m);
		pthread_cond_signal (&c_cons);
		
	}
	
}
void *consumer(void *param)
{
	int i;
	while (1) {
		pthread_mutex_lock (&m);
		if (celulasOcupadas < 0) exit(1);
		while (celulasOcupadas == 0){
			printf(YELLOW "||||[ BUFFER VAZIO ]||||\n\n\n" RESET);
			pthread_cond_wait (&c_cons, &m);
		}
		i = buffer[indiceDeLeitura];
		printf("CONSUMIU: %d\n\n",i );
		buffer[indiceDeLeitura] = -1;			
		indiceDeLeitura = (indiceDeLeitura+1) % BUF_SIZE;
		celulasOcupadas--;
		imprime(indiceDeLeitura,BUF_SIZE);
		sleep((int)(drand48() * 4.0));
		pthread_mutex_unlock (&m);
		pthread_cond_signal (&c_prod);
		
	}
	
}
void *inicializaBuffer(){
	int i;
	for (i = 0; i < BUF_SIZE; i++)
	{
		buffer[i] = -1;
	}
}

void *imprime(int inicio, int fim){
	int i;
	printf(CYAN "BUFFER: " RESET);
	printf(GREEN "|");

	for (i = inicio; i < fim; i++)
	{
		printf(GREEN " %d |" RESET,buffer[i]); 		
	}
	printf("\n\n");
}


