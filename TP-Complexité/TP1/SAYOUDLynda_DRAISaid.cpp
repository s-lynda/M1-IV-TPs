#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* Remplissage des tableaux : 
1- avec des données triées
2-avec des données triées en ordre inverse.
3- avec des données aléatoires.
*/
void Remplir_sorted_values(int T[],int N)
{
	int i;
	for ( i = 0; i < N ; i ++)
	{
		T[i] = i ;
	}
	
}

void Remplir_reversed_values(int T[],int N)
{
	int i;
	for ( i = 0; i < N ; i ++)
	{
 		T[i] = N - i ;
	}
	
}

void Remplir_Random(int T[],int N)
{
	int i;
    for(i=0; i<N; i++)
    {
        T[i] = (int)rand()%N;
    }
}

//Affichage

void Affichage_tab(int T[],int N)
{int i;
	for(i=0;i<N;i++)
	{
		printf(" %d |",T[i]);
				
	}
}


//Fonction --Tri par insertion --

double InsertionSort(int T[], int N)
{
     int j,i,x;
     double temps_exe;
     
     clock_t start=clock();
	 j=1;
     while(j<N)
      {
      	x=T[j];
      	i=j;
      	
      	while(i>0 && T[i-1]>x)
      	{
      	 T[i]=T[i-1];
		 i=i-1;	
		}
		T[i]=x;
		j++;
	  }
	  
    clock_t end= clock();
    temps_exe = (double)(end-start)/CLOCKS_PER_SEC;
    
    return temps_exe;
}

//Fonction --Tri à bulles--
double bubble_sorted(int T[],int N)
{
	int j,i,x,TRI;
    double temps_exe;
     
    clock_t start=clock();
    
	TRI=0;
	while(TRI==0)
	{
		TRI=1;
		for(i=1;i<=N-1;i++)
		{
			if(T[i]>T[i+1])
			{
				x=T[i];
				T[i]=T[i+1];
				T[i+1]=x;
				TRI=0;
			}
		}
		N=N-1;
	}
	 clock_t end= clock();
    temps_exe = (double)(end-start)/CLOCKS_PER_SEC;
    
    return temps_exe;	
}


void fusionner(int tab[],int deb1,int fin1,int fin2)
{     
   int *table1;
   int deb2=fin1+1;
   int compt1=deb1;
   int compt2=deb2;
   int i;
   int A=fin1-deb1+1;
   table1=(int*)malloc(A*sizeof(int));
  
       for(i=deb1;i<=fin1;i++)
           {
           table1[i-deb1]=tab[i];
           }
                      
       for(i=deb1;i<=fin2;i++)
           {       
           if (compt1==deb2)
               {
               break;
               }
           else if (compt2==(fin2+1))
               {
               tab[i]=table1[compt1-deb1];
               compt1++;
               }
           else if (table1[compt1-deb1]<tab[compt2])
               {
               tab[i]=table1[compt1-deb1];
               compt1++;
               }
           else
               {
               tab[i]=tab[compt2];
               compt2++;
               }
           }
       free(table1);      
}
      
 
void fusionsort_1(int tableau[],int deb,int fin)
       {
       if (deb!=fin)
           {
           int milieu=(fin+deb)/2;
           fusionsort_1(tableau,deb,milieu);
           fusionsort_1(tableau,milieu+1,fin);
           fusionner(tableau,deb,milieu,fin);
           }
       }
 
double FusionSort(int tab[],int N)
    {
       double temps_exe;
	   clock_t start=clock();
       if (N>0)
           {
           fusionsort_1(tab,0,N-1);
           }
      clock_t end= clock();
 	  temps_exe = (double)(end-start)/CLOCKS_PER_SEC;
   
 	  return temps_exe;	
    }



//Fonction --Tri Rapide Itératif--

typedef struct elt elt;
typedef elt* Tptr;

typedef struct elt
{
    int val;
    elt* adr;
};

typedef elt* Pile;

void LibererP (Tptr p)
{
    free(p);
}

void Aff_val(Tptr p, int v)
{
    p->val = v;
}

void Aff_adr(Tptr p, Tptr q)
{
    p->adr = q;
}

int Valeur (Tptr p)
{
    return p->val;
}

Tptr Suivant (Tptr p)
{
    return p->adr;
}

Tptr Allouer(Tptr* p)
{
    *p = (Tptr)malloc(sizeof(elt));
}

void CreerPile(Pile* P)
{
    *P = NULL;
}

int PileVide(Pile P)
{
    return (P == NULL);
}

void Empiler(Pile* P, int val)
{
    Tptr Q;

    Allouer(&Q);
    Aff_val(Q, val);
    Aff_adr (Q, *P);
    *P = Q;
}

void Depiler (Pile* P, int* val)
{
    Tptr Sauv;

    if (!PileVide(*P))
    {
        Sauv = *P;
        *val = Valeur(*P);
        *P = Suivant (*P);
        LibererP(Sauv);
    }
    else
        printf("\nPile vide ...\n");
}

void AffichPile (Pile P)
{
    Tptr Q;

    if (!PileVide(P))
    {
        Q = P;
        printf("\n");
        while (Q != NULL)
        {
            printf(" %d |\n", Valeur(Q));
            Q = Suivant(Q);
        }
        printf("\n");
    }
    else
        printf("\nPile vide ....\n");
}

void permuter (int* a , int* b)
    {
        int tmp;
        tmp=*a;
        *a=*b;
        *b=tmp;
    }
    
double quickSort(int T[], int N)
{
   double temps;
   clock_t start, stop;
   
   int min,max,i,pivot,p_pivot;
   Pile P;
   
   start = clock();
   CreerPile (&P);
   Empiler(&P,0);
   Empiler (&P,N-1);
 
   while (!PileVide(P))
   {
       Depiler (&P, &max);
       Depiler (&P, &min);
       pivot=T[max];
       p_pivot=min;
       for (i=min; i<=max-1; i++)
       {
           if (T[i]<=pivot)
           {
               permuter(&T[i],&T[p_pivot]);
               p_pivot++;
           }
       }
       permuter(&T[p_pivot], &T[max]);
       if (p_pivot+1 <max)
       {
           Empiler(&P,p_pivot+1);
           Empiler(&P,max);
       }
       if (p_pivot-1>min)
       {
           Empiler(&P,min);
           Empiler(&P,p_pivot-1);
       }
   }
   stop = clock();
   temps=(double)(stop-start)/CLOCKS_PER_SEC;
   return temps;
}
//-----Fonction tri par tas
void MakeHeap(int arr[], int n, int i)
{
   // Find largest among root, left child and right child
   int largest = i;
   int left = 2 * i + 1;
   int right = 2 * i + 2;
    if (left < n && arr[left] > arr[largest])
     largest = left;
    if (right < n && arr[right] > arr[largest])
     largest = right;
    // Swap and continue heapifying if root is not largest
   if (largest != i) {
     permuter(&arr[i], &arr[largest]);
     MakeHeap(arr, n, largest);
   }
 }
  
double HeapSort(int arr[], int n)
 {
   double temps;
   clock_t start, stop;
   start = clock();
   // Build max heap
   for (int i = n / 2 - 1; i >= 0; i--)
     MakeHeap(arr, n, i);
    // Heap sort
   for (int i = n - 1; i >= 0; i--)
    {
     	permuter(&arr[0], &arr[i]);
      // Heapify root element to get highest element at root again
     MakeHeap(arr, i, 0);
   }
   stop =clock();
   temps = (double)(stop-start)/CLOCKS_PER_SEC;
   return temps;
 }


int main()
{
	int N,i,choice;
	double T1,T2,T3;
	
	//Taille du tableau >=2
	
	do{
		printf("\n-Entrer la taille du tableau:");
		scanf("%d",&N);	
	}while(N<2);
	
	int *Tab=(int*)malloc(N*sizeof(int));
    while(1)
	{
	
	printf("\n\n-Veuillez choisir le type de tri:");
	printf("\n 1 :Tri par insertion");
	printf("\n 2 :Tri a Bulles");
	printf("\n 3 :Tri Fusion");
	printf("\n 4 :Tri rapide(quick sort)");
	printf("\n 5 :Tri par tas (Heapsort)");
	printf("\n 6 :Exit");
	printf("\n..Tapez votre choix:");
	scanf("%d",&choice);

	//Lire la taille du tableau 
	switch(choice)
	{
		case 1:
			//tri par insertion
			printf("\n-----Tri par insertion------");
			printf("\n-Case 1: Tableau avec des donnees triees------");
			printf("\n--Original table --\n");
			Remplir_sorted_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T1=InsertionSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (sorted data): %f", T1);
			printf("\n-Case 2: Tableau avec des donnees triees en ordre inverse------");
			printf("\n--Original table --\n");
			Remplir_reversed_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T2=InsertionSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (insorted data): %f", T2);
			printf("\n-Case 3: Tableau avec des donnees aleatoires------");
			printf("\n--Original table --\n");
			Remplir_Random(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T3=InsertionSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (random data): %f", T3);
			printf(" T1=%f,T2=%f,T3=%f",T1,T2,T3);
			break;
		case 2:
			printf("\n-----Tri a Bulles------");
			printf("\n-Case 1: Tableau avec des donnees triees------");
			printf("\n--Original table --\n");
			Remplir_sorted_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T1=bubble_sorted(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (sorted data): %f", T1);
			printf("\n-Case 2: Tableau avec des donnees triees en ordre inverse------");
			printf("\n--Original table --\n");
			Remplir_reversed_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T2=bubble_sorted(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (insorted data): %f", T2);
			printf("\n-Case 3: Tableau avec des donnees aleatoires------");
			printf("\n--Original table --\n");
			Remplir_Random(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T3=bubble_sorted(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (random data): %f", T3);
			printf(" T1=%f,T2=%f,T3=%f",T1,T2,T3);
			break;
			
		case 3:
			printf("\n-----Tri Fusion------");
			printf("\n-Case 1: Tableau avec des donnees triees------");
			printf("\n--Original table --\n");
			Remplir_sorted_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T1=FusionSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (sorted data): %f", T1);
			printf("\n-Case 2: Tableau avec des donnees triees en ordre inverse------");
			printf("\n--Original table --\n");
			Remplir_reversed_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T2=FusionSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (insorted data): %f", T2);
			printf("\n-Case 3: Tableau avec des donnees aleatoires------");
			printf("\n--Original table --\n");
			Remplir_Random(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T3=FusionSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (random data): %f", T3);
			printf("\nT1=%f,T2=%f,T3=%f",T1,T2,T3);
			break;
			
		case 4:
			printf("\n-----Tri Rapide------");
			printf("\n-Case 1: Tableau avec des donnees triees------");
			printf("\n--Original table --\n");
			Remplir_sorted_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table--\n");
			T1=quickSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (sorted data): %f", T1);
			printf("\n-Case 2: Tableau avec des donnees triees en ordre inverse------");
			printf("\n--Original table --\n");
			Remplir_reversed_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T2=quickSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (insorted data): %f", T2);
			printf("\n-Case 3: Tableau avec des donnees aleatoires------");
			printf("\n--Original table --\n");
			Remplir_Random(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T3=quickSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (random data): %f", T3);
			printf("\nT1=%f,T2=%f,T3=%f",T1,T2,T3);
			
			break;
			
		case 5:
			printf("\n-----Tri par tas------");
			printf("\n-Case 1: Tableau avec des donnees triees------");
			printf("\n--Original table --\n");
			Remplir_sorted_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T1=HeapSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (sorted data): %f", T1);
			printf("\n-Case 2: Tableau avec des donnees triees en ordre inverse------");
			printf("\n--Original table --\n");
			Remplir_reversed_values(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T2=HeapSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (insorted data): %f", T2);
			printf("\n-Case 3: Tableau avec des donnees aleatoires------");
			printf("\n--Original table --\n");
			Remplir_Random(Tab,N);
			Affichage_tab(Tab,N);
			printf("\n--Sorted table --\n");
			T3=HeapSort(Tab,N);
			Affichage_tab(Tab,N);
			printf("\nTemps d'execution (random data): %f", T3);
			printf("\nT1=%f,T2=%f,T3=%f",T1,T2,T3);
			break;
		case 6: 
			exit(0);
		
		default:
		printf("\n--invalid number!!...");	
	}
	}

      return 0;
}
