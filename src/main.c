#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>


pthread_mutex_t trava;
typedef struct particao{
	int * thrtask[4];
	int n[4];
}particao;

typedef particao * Part;



//variaveis globais

int cafe[4],   ocupada[4]={0, 0, 0, 0};
Part list;

void merge( int * destino, int * v1, int n1, int * v2, int n2){
	if(n1 >0 && n2>0){

		if(v1[0]>v2[0]){
			destino[0]=v2[0];
			merge(destino+1, v1, n1, v2+1, n2-1);
		}

		else{
			destino[0]=v1[0];
			merge(destino+1, v1+1, n1-1, v2, n2);
		}
	}
	else if(n1>0)
	{
		destino[0]=v1[0];
		merge(destino+1, v1+1, n1-1, v2, n2);
	}
	else if(n2>0)
	{
		destino[0]=v2[0];
		merge(destino+1, v1, n1, v2+1, n2-1);
	}
}

void copiar(int *destino, int *origem,int n){
	int i;
	for(i=0; i< n; i++){
		destino[i]=origem[i];
	}
}

void merge_sort( int * v, int n){
	if(n>1){
		int i, *v1, *v2;
		v1=malloc(sizeof(int)*n/2);
		v2=malloc(sizeof(int)*n/2+n%2);
		copiar(v1, v, n/2);	
		copiar(v2, v+n/2, n/2+n%2);	
		merge_sort(v1, n/2);
		merge_sort(v2, n/2+n%2);
		merge(v, v1, n/2, v2, n/2+n%2);
		free(v1);
		free(v2);
	}
}


void *worker(void *arg) {
	int* pttid=(int * ) arg;
	int tid=*pttid, n, *vetor;
	free(pttid);

	pthread_mutex_lock(&trava); 
	vetor=list->thrtask[tid];
	n=list->n[tid];
	pthread_mutex_unlock(&trava); 
	merge_sort(vetor, n);

	return NULL;
}


int main(int argc, char **argv) {

	pthread_t thread[4];
	int i,j=0, n=0 , nthread, *tid;
	int vetor[100000];
	int * merge1, *merge2, *mergefinal;

	char c=0, s[100];

	//Le a entrada do programa ate EOL
	while(c != '\n'){
		//le numero inteiro para numero
		i=0;
		scanf("%c", &c);
		while('0'<= c && c <= '9'){
			s[i]=c;		
			i++;
			scanf("%c", &c);
		}
		s[i]='\0';
		vetor[n]=atoi(s);
		n++;
		//coloca lista num vetor
	}


	list=malloc(sizeof(particao));
	//faz vetores subdivisoes para as threads
	for(j=0; j<4; j++){
		list->n[j]=n/4 +( j==3 ? n%4 : 0);
		list->thrtask[j]=malloc(sizeof(int)*list->n[j] );
		copiar(list->thrtask[j], vetor+j*(n/4),list->n[j]);
	}

	//faz novas threads e passa as ids delas
	for(j=0; j<4; j++){
		tid=malloc(sizeof(int));
		*tid=j;
		pthread_create(&thread[j], NULL, worker, tid);
	}
	j=0;


	//printf("Encerrado\n");


	for(i=0; i<4; i++){
		pthread_join(thread[i], NULL);
	}


	//merge final
	merge1=malloc(sizeof(int)*n/2);
	merge(merge1, list->thrtask[0], n/4, list->thrtask[1], n/4);

	merge2=malloc(sizeof(int)*n/2+n%4);
	merge(merge2, list->thrtask[2], n/4, list->thrtask[3], n/4+n%4);

	merge(vetor, merge1, 2*(n/4), merge2, 2*(n/4)+n%4);

	for(j=0; j<n-1; j++)
		printf("%d ", vetor[j]);
	printf("%d\n", vetor[j]);

	for(j=0; j<4; j++){
		free(list->thrtask[j]);
	}
	free(merge1);
	free(merge2);
	free(list);

	return 0;
}
