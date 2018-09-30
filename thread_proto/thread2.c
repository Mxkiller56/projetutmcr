/* pseudo-threading thing */
#include <stdio.h>
#include <stdlib.h>

/* malloc standard
int *ptr = malloc(sizeof(int));
*ptr = smth;
free(ptr);
*/

#define CONTINUE 0
#define DESTROY -1
// pas d'allocation dynamique pour le moment
#define MAXTHREADS 100

/* global variables */
int threadcnt=0;
int (*threadlist[MAXTHREADS])(void); // liste de threads (les fonctions retournent un int)

/* struct thread (ring) */
struct thread {
  int (**fun)(void);
  struct thread *next;
  struct thread *prev;
}

/* functions definitions */
int addthread(int (*fun)(void));
int delthread_by_num(int);
void runthreads (void);

int thread_u (void){printf("u"); return DESTROY;}
int thread_g (void){printf("g"); return DESTROY;}
int thread_o (void){printf("o"); return CONTINUE;}
int thread_z (void){static int i = 0; if (++i>5) {delthread_by_num(0); delthread_by_num(1); return DESTROY; }}
		   
int main (int argc, int **argv){
  // les threads peuvent ajouter
  // elles-memes des threads
  // ou detruire le leur (ne plus le faire tourner)
  addthread(&thread_u);
  addthread(&thread_o);
  addthread(&thread_g);
  addthread(&thread_z);
  runthreads();
  return 0;
}

/* ajouter un thread en cours de route (prend en arg un pointeur sur fonction) */
int addthread (int (*fun)(void)){
  threadlist[threadcnt] = fun;
  threadcnt++;
}

int delthread_by_num (int threadnum){
  int i;
  //supprimer un thread
  // free(threadlist[threadcnt]); // attention au use-after free, on libere la fin
  // on tasse pour eviter le tableau gruyere, faut pas que la liste soit trop
  // longue TODO: optimiser avec une liste chainee
  for (i=threadnum; i<threadcnt; i++){
    threadlist[i] = threadlist[i+1];
  }
  threadcnt--;
}

void runthreads (void){
  //parcourt le threadlist
  //et appelle les fonctions
  //ne retourne que quand il n'y a plus de threads
  int i = 0;
  for (i=0; threadcnt > 0; (i+1>threadcnt-1) ? i=0 : i++){
    if ((*threadlist[i])() == DESTROY)
      delthread_by_num(i);
  }
}
