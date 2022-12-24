#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void Hanoi_tours(int N, char Dep, char Arr, char Inter) 
{
	//N -> nombre de disques � deplacer
    if (N == 1)
      printf("Disque 1 de %c a %c \n" , Dep , Arr);
    else {
      // du tour de depart vers l'arriv�e
      Hanoi_tours(N - 1, Dep, Inter, Arr);
      printf("Disque %d de %c a %c \n", N , Dep ,Arr);
      // du tour intermidiaire � l'arriv�
      Hanoi_tours(N - 1, Inter, Arr, Dep);
    }
}

int main() {
    int N;
    double temps_exe;
    
    clock_t start, end;
    do {
    	printf("\n-Entrer le nombre de disques:");
    	scanf("%d",&N);
	}while(N<1);
	//Les Tours sont A B C 
	start = clock();
    Hanoi_tours(N, 'A', 'C', 'B');
    end = clock();
    temps_exe = (double)(end-start)/ CLOCKS_PER_SEC;
    printf("\n- Le temps d'execution est: %f \n", temps_exe);
   
}
