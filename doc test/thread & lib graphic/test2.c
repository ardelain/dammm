#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>     
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include "graphic.h"

#define N 200

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	int x,y,color;
} Point;

typedef struct{
	int v;
	Point* tPunto;
	int nb_cpu;	
} Jack;

void *pointThread(void *arg){
	Jack *a = (Jack*)arg;
	Point *elPunto = a->tPunto;
	int j = 0;
	while(1)
	{	
		for(j = a->v; j < N; j = j + a->nb_cpu){
			pthread_mutex_lock(&m);
			elPunto[j].x+=g_random()%2?1:-1;
			elPunto[j].y+=g_random()%2?1:-1;
			pthread_mutex_unlock(&m);		
		}
		g_msleep(20);	
	}
	pthread_exit (NULL);
}

int main() {
	int i;
	int nb_cpu = get_nprocs();
 	pthread_t th1[nb_cpu];
 	Jack j[nb_cpu];
 	Point shmaddr[N];

    for(i=0; i<N; ++i) {
      	shmaddr[i].x=g_random()%(G_WIDTH-200)+100;
		shmaddr[i].y=g_random()%(G_HEIGHT-200)+100;
		shmaddr[i].color=g_random()%G_NB_COLORS;
	}
	for(i=0; i<nb_cpu; ++i) {
		j[i].v = i;
		j[i].tPunto = shmaddr;
		j[i].nb_cpu = nb_cpu;
		if(pthread_create (&th1[i], NULL, pointThread, &j[i]) < 0) {
		      fprintf (stderr, "pthread_create error for thread %d\n", i);
		      exit (1);
		}
	}
	g_init();
	while(1){
        g_clear();
        pthread_mutex_lock(&m);
		for(i=0; i<N; ++i){ 
			g_draw(shmaddr[i].x, shmaddr[i].y, shmaddr[i].color);
		}
		pthread_mutex_unlock(&m);
		g_flush();
        g_msleep(20);
	}
	return 0;
}

