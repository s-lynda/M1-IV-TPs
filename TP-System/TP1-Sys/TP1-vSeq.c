#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<sys/types.h>
#include<string.h>

// chaine de test: (((A+B)*C)-(((D-(F/G))*(H+(K*L)))/((M-N)*O))))
//((A+B)*(C-(D/E))
#define N 100

//--Fonction qui lit l'expression arthmétique --

char* Read_Expr()
{
    char *expr = malloc(sizeof(char)*N);

    printf("\n -Veuillez introduire votre expression:");
    scanf("%s",expr);
    printf("\n La chaine lue est : %s",expr);
    
    return expr;
}


//--Fonction qui renvoie l' indice de l'opérateur centrale--
int operateur_centrale(char expression[])
{
  int i=1, cpt_prth=0;
  int index_op_ctrl=0;
  
  
   do{
   	if(expression[i]=='(')
    	{
      	cpt_prth=cpt_prth+1;
    	}
    
    	if(expression[i]==')')
    	{
      	 cpt_prth=cpt_prth-1;
        }
        
        i=i+1;
     
    }while(cpt_prth!=0);
    
   index_op_ctrl=i;
   
   //printf("\n-L operateur centrale d indice %d  est : %c",index_op_ctrl,expression[index_op_ctrl]);
  
  return index_op_ctrl;
}


//--Fonction de calcule du nbre d'operateur--

int count_op(char expr[])
{
    int i, cpt_op=0;
    for(i=0; i<strlen(expr); i++)
    {
        if(expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/')
        {
            cpt_op=cpt_op+1;
        }
    }
    //printf("\n-Nombre d'operateur:%d",cpt_op);
    return cpt_op;
}

//--Generer une tache <-> générer node--
void genere_noeud(int index_node)
{
    printf("\nT%d:",index_node);

}

//--Fonction Generer précédence--

void generer_precedence(int index_node, int index_pere)
{
  printf("T%d < T%d\n", index_node, index_pere);
  
}


//--Fonction Générer une tache --

void generer_tache(char expr[],int index_node, int op_ctrl,int j)

{  char gauche[10], droite[10];

    //coté gauche , si la fin de la partie gauche est ) donc c une expr 
    if( expr[op_ctrl-1]== ')' )
    {
        sprintf(gauche, "M%d",index_node+1);
    }
    //cas d'une variable ou constante 
    else
    {
        sprintf(gauche, "%c", expr[op_ctrl-1]);
    }
 
    //coté droit , si le debut de la partie droite est ( donc c une expr
    //j--> nbr d'operateur
    if( expr[op_ctrl+1]== '(' )
    {
        sprintf(droite,"M%d",index_node+j+1);
    }
    else
    {
        sprintf(droite,"%c",expr[op_ctrl+1]);
    }
    //Exemple: M1 := M2*M3
    
    printf("M%d := %s %c %s",index_node,gauche,expr[op_ctrl],droite);
    
}

//----------- Fonction générer ---------
void generer(char expr[], int index_node, int index_pere)
{ 
    int j, op_ctrl,i=0,x=0,k=0;
    char FilsGauche[50];
    char FilsDroit[50];
    
    //Generer le 1 er noeud
   genere_noeud(index_node);
   op_ctrl=operateur_centrale(expr);
   
    if(expr[op_ctrl-1] == ')')
    {
     //On est dans le coté gauche      
        for(i = 1; i < op_ctrl; i++)
        {
            FilsGauche[i-1] = expr[i];
        }
        generer(FilsGauche,index_node+1, index_node);
    }
    j =count_op(FilsGauche);
    //coté droit
    if(expr[op_ctrl+1] == '(')
    {
        for(k=op_ctrl+1; k<strlen(expr)-1; k++)
        {
            FilsDroit[x] = expr[k];  
            x++;
        }
        
        generer(FilsDroit, index_node+j+1, index_node);
    }

    generer_tache(expr,index_node,op_ctrl,j);
    
    printf("\n");
   generer_precedence(index_node, index_pere);
}



void main()
{
   char *expr;
   int op_ctrl,cpt_op=0;
    expr=Read_Expr();
    op_ctrl=operateur_centrale(expr);
    //cpt_op=count_op(expr);
     printf("\n");
   //generer une tache
   //generer_tache(expr,1,op_ctrl,1);
   
   //générer
   generer(expr,1,0);
   
   

}
