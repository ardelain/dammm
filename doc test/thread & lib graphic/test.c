#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h> 
#include "graphic.h"
 
#define N 200
#define NB 8

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
 
typedef struct {
	int x,y,color;
} Point;
 
typedef struct {
	Point* p;
	int num;
	int nb_cpu;
} tableauThread;
 
void *my_thread_process(void * arg){
	tableauThread* tt = (tableauThread*)arg;
	int i;
	
	while(1){
		for(i= tt->num ; i<N; i = i+tt->nb_cpu) {
			pthread_mutex_lock(&mut);
			tt->p[i].x += g_random()%2?1:-1;
			tt->p[i].y += g_random()%2?1:-1;
			pthread_mutex_unlock(&mut);
		
		}
			g_msleep(20);
	}
	exit(1);
}
 
int main() {
	Point points[N];
	tableauThread tt[get_nprocs()];
	//tt.p = points;
	//tt.nb_cpu = get_nprocs();
	//fprintf (stderr, " procs %d\n", tt.nb_cpu);
	
	int i , j;
	pthread_t threads[N];
	/* initialisation aléatoire des N points */
	for(i=0; i<N; ++i) {
		points[i].x=g_random()%(G_WIDTH-200)+100;
		points[i].y=g_random()%(G_HEIGHT-200)+100;
		points[i].color=g_random()%G_NB_COLORS;
	}
	
	//Initialisation des threads
	for(i=0; i<get_nprocs() ; i++) {
		tt[i].p = points;
		tt[i].nb_cpu = get_nprocs();
		tt[i].num = i;
		if (pthread_create (&threads[i], NULL, my_thread_process, &tt[i]) < 0) {
			fprintf (stderr, "pthread_create error for thread %d\n", i);
			exit (1);
		}
	}
	g_init();
 
	while(1) {
		/* mise à jour des points */

		/* Affichage des points */
		pthread_mutex_lock(&mut);
		g_clear();
		for(i=0; i<N; ++i) 
			g_draw(points[i].x, points[i].y, points[i].color);
 
		pthread_mutex_unlock(&mut);

		g_msleep(20);
		g_flush();

	}
 
	return 0;
}


