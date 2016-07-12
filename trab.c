#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define BUF_SIZE 4

int buffer[BUF_SIZE];
int consumidos[BUF_SIZE];
int produzidos[BUF_SIZE];
int add=0;
int rem=0;
int num=0;
pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_cons=PTHREAD_COND_INITIALIZER;
pthread_cond_t c_prod=PTHREAD_COND_INITIALIZER;

void *producer(void *param);
void *consumer(void *param);
void *inicializaBuffer();
void *imprimeConsumidos();
void *imprimeProduzidos();

main (int argc, char *argv[])
{
	inicializaBuffer();
	imprimeConsumidos();

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
	printf("Funcao principal antes join\n");
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	printf("Funcao principal depois join\n");
}
void *producer(void *param)
{
	int i;

	while(1) {
		i = (int)(drand48() * 100.0);	
		
		pthread_mutex_lock (&m);
		while (num == BUF_SIZE)
			pthread_cond_wait (&c_prod, &m);
		//printf("produzindo - %d \n",i);
		buffer[add] = i;
		printf("produziu: %d\n",i );
		produzidos[add] = i;
		consumidos[add] = i;
		imprimeProduzidos();
		add = (add+1) % BUF_SIZE;
		num++;
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
		if (num < 0) exit(1);
		while (num == 0)
			pthread_cond_wait (&c_cons, &m);
		i = buffer[rem];
		printf("consumiu: %d\n",i );
		consumidos[rem] = 0;
		imprimeConsumidos();
		
		rem = (rem+1) % BUF_SIZE;
		num--;
		sleep((int)(drand48() * 4.0));
		pthread_mutex_unlock (&m);
		pthread_cond_signal (&c_prod);
		
	}
	
}
void *inicializaBuffer(){
	int i;
	for (i = 0; i < BUF_SIZE; i++)
	{
		buffer[i] = 0;
		consumidos[i] = 0;
		produzidos[i] = 0;

	}
}

void *imprimeConsumidos(){
	int i;
	printf("BUFFER: ");
	for (i = 0; i < BUF_SIZE; i++)
	{
		printf(" %d    ",consumidos[i]); 		
	}
	printf("\n");
}

void *imprimeProduzidos(){
	int i;
	printf("BUFFER: ");
	for (i = 0; i < BUF_SIZE; i++)
	{
		printf(" %d - ",produzidos[i]); 		
	}
	printf("\n");
}

