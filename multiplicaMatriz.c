/* Multiplicacao de matriz-vetor (considerando matrizes quadradas) */
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

float *matA; //matriz de entrada A
float *matB; //matriz de entrada B
float *saida; //matriz de saida
int nthreads; //numero de threads

typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int dim; //dimensao das estruturas de entrada
} tArgs;

//funcao que as threads executarao
void * tarefa(void *arg) {
    int aux;
    tArgs *args = (tArgs*) arg;
    for(int i=args->id; i<args->dim; i+=nthreads){
        for(int j=args->id; j<args->dim; j+=nthreads){ 
            aux = 0;
            for(int k=0; k<args->dim; k++)
                aux+= matA[i * args->dim + k] * matB[k * args->dim + i];
            saida[i*args->dim + j] = aux;
        }
    }
    pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char* argv[]) {
   int dim; //dimensão das matrizes de entrada
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e dimensao
   double inicio, fim, delta;
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   if(argc<3) {
      printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
      return 1;
   }
   dim = atoi(argv[1]);
   nthreads = atoi(argv[2]);
   if (nthreads > dim) nthreads=dim;

   //alocacao de memoria para as estruturas de dados
   matA = (float *) malloc(sizeof(float) * dim * dim);
   if (matA == NULL) {printf("ERRO--malloc\n"); return 2;}
   matB = (float *) malloc(sizeof(float) * dim * dim);
   if (matB == NULL) {printf("ERRO--malloc\n"); return 2;}
   saida = (float *) malloc(sizeof(float) * dim * dim);
   if (saida == NULL) {printf("ERRO--malloc\n"); return 2;}

   //inicializacao das estruturas de dados de entrada e saida
    for(int i=0; i<dim; i++) {
        for(int j=0; j<dim; j++){
            matA[i*dim+j] = 1;   //equivalente mat[i][j]
            matB[i*dim+j] = 1; 
            saida[i*dim+j] = 0;
        }
    }
   GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo inicializacao:%lf\n", delta);

   //multiplicacao da matriz pelo vetor
   GET_TIME(inicio);
   //alocacao das estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}
   args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
   if(args==NULL) {puts("ERRO--malloc"); return 2;}
   //criacao das threads
   for(int i=0; i<nthreads; i++) {
      (args+i)->id = i;
      (args+i)->dim = dim;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   } 
   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }
    //versão sequencial
    // int aux;
    // for(int i=0; i<dim; i++){
    //     for(int j=0; j<dim; j++){ 
    //         aux = 0;
    //         for(int k=0; k<dim; k++)
    //             aux+= matA[i * dim + k] * matB[k * dim + i];
    //         saida[i*dim + j] = aux;
    //     }
    // }
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo multiplicacao:%lf\n", delta);

   //exibicao dos resultados
//    puts("Vetor de saida:");
//    for(int i=0; i<dim; i++){
//     for(int j=0; j<dim; j++)
//         printf("%.1f ", saida[i*dim + j]);
//     puts("");
   //}

   //liberacao da memoria
   GET_TIME(inicio);
   free(matA);
   free(matB);
   free(saida);
   free(args);
   free(tid);
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo finalizacao:%lf\n", delta);

   return 0;
}
