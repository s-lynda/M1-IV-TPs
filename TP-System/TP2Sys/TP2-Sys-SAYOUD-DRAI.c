#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

/*----Constantes-----*/
#define NB_Etud 10
#define N 1
#define F 2
//F et N sont les types des etudiants


/*------déclaration des structures ------------*/

typedef struct request
{
	int   typeEtud; /* 1:F, 2:N */
	int   reqtype; /* 1:inscritpion , 2:désistement , 0: terminé */
	int   nom_req;
	float moy;  //moy de l'etudiant 
	int   filiere;  //f1 || f2 || f3 
	
}request;

typedef struct Response
{
	int      nom_res;
	int      filiere_aff;
	int 	 typerep; // 1: Inscrit , 2: Complet , 3:Exclu , 4:Negative
	int      typeEtudiant; // 1: N , 2:F
	int 	 filiere_req; //f1, f2 ou f3, -1
	float moy_rep;
}Response;

typedef struct Gerant{
	int      typeStudent;// 1:F ou 2:N
	int 	 typeresp;   // 1: Inscrit , 2: Complet , 3:Exclu , 4:Negative
	int      nomE;
	float    moy;
	int      fil1; //filiere demandé,
	int      fil2;//filiere affecté respectivement
	int 	 nbr_desist; 		
}Gerant;

typedef struct filiere
{
	int     nom; 
	int     nbp;  //nombres places total
	int     nbpr; // nombres de places pleines(prises)
	float   max;  //moy min de la filiere
	
}filiere;

//Tampon comme une BAL qui contient  les reponses du traitement fait par le gerant que les processus EtudF & etudF vont récuperer

struct boite
{
	long    mesg_type;
	Response rep; 
	
}QRep;


//memoire partagé entre processus <--> Tampon ou les 2 processus depose leur requetes 

typedef struct shared_memory
{
    request SReq[4];
    int x; //represente lindice de l'enregistrement dans le tampon
    
}shared_memory;

shared_memory *shared_mem;

int Qrepid, Sreqid; 

/*--------Déclaration  des sémaphores------------*/

sem_t *mutex1,*nplein, *nvide;

/*--------Initialisation des sémaphores------------*/

void init_sem()
{
   key_t shm_key = ftok("sharedmemory.txt",65);
   key_t msg_key = ftok("messagequeue.txt",65);
   
   if((Sreqid = shmget(shm_key, sizeof(shared_mem), IPC_CREAT|0644))<0)
   {
      printf("shmget error");
      exit(1);
   }

   if((shared_mem = shmat(Sreqid,0,0))==(void*)-1)
   {
       printf("shmat error");
       exit(1);
   }

   Qrepid = msgget(msg_key,0666|IPC_CREAT);
   
  //initialisation
  mutex1 = sem_open("mutex1",O_CREAT  | O_EXCL, 0644,1); //mutex pourque EtudN et EtudF ne dépose pas au meme temps
  nplein= sem_open("nplein",O_CREAT | O_EXCL, 0644,0);  //pourqu'on le tampon est plein les processus attendent
  nvide = sem_open("nvide",O_CREAT | O_EXCL, 0644,4); //pour qu'on le tampon possede des cases vides ,les processus peuvent deposer 
  
  
}

/*1-Remplir le fichier EtudiantN-*/

// N --> Les etudiants qui n'exigent pas 

void filetudN()
{
request req; 
req.reqtype = 1; //inscription 
req.typeEtud= N; //N=2
req.nom_req=1;
FILE* f1 = fopen("FichN.txt", "w");

 for(int i=0; i<NB_Etud;i++)
   {
	req.moy= rand()%20;
	fwrite(&req, sizeof(struct request),1, f1);
	req.nom_req++;
	
   }

req.reqtype = 0; //terminé 

fwrite(&req, sizeof(struct request),1, f1);
fclose(f1);

}

/*2-Remplir le fichier des etudiantsF--*/

//F --> Exige le choix de la filiere 

void filetudF()
{  
   request req; 
   req.reqtype = 1; //inscription 
   req.typeEtud= F; //F=2
   req.nom_req=11;	
   FILE* f1 = fopen("FichF.txt", "w");

   for(int i=0; i<NB_Etud;i++)
    {
	req.filiere =rand()%3+1;
	req.moy= rand()%20;
	fwrite(&req, sizeof(struct request),1, f1);	
	req.nom_req++;
    }

req.reqtype = 0; //terminé 

fwrite(&req, sizeof(struct request),1, f1);

fclose(f1);

}
//fonction save_response 

Response save_response(Response rep,Gerant adm, int fil_aff, int req_type)
{
        rep.nom_res= adm.nomE;
        rep.typeEtudiant = adm.typeStudent;
        rep.filiere_req=adm.fil1;
        rep.moy_rep=adm.moy;
        rep.filiere_aff=fil_aff;
        rep.typerep=req_type;
        return rep;

}
//3-Fonction de traitement d'une requete d'un etudiant 

Response request_treatment(Gerant adm, filiere fil1, filiere fil2, filiere fil3)
{
	Response rep;
	
	//--Traitement d'une demande d'un etudiant  de type N
      	if(adm.typeStudent == N)
      	{
      	     //Vu N n'exige pas donc on commence par la filiere qui possede la plus grande moyenne d'acces 
    	      if(adm.moy>=fil1.max && fil1.nbp > fil1.nbpr)
    	       {
                 	fil1.nbpr++;
                        rep=save_response(rep,adm,1,1);//Inscrit
            
    	       }else if(adm.moy< fil1.max && adm.moy>=fil2.max && fil2.nbp> fil2.nbpr) 
    	       //le cas ou sa moyenne ne satisfait pas la moy de la filiere 1 on passe à la filiere suivante   	
    	       {
    	           fil2.nbpr++;
    	           
    	           rep=save_response(rep,adm,2,1);//Inscrit
   
    	       }else if(adm.moy< fil2.max && adm.moy>=fil3.max && fil3.nbp> fil3.nbpr)
    	       {
    	         fil3.nbpr=fil3.nbpr++;
    	          rep=save_response(rep,adm,3,1);//Inscrit
 
    	       }else 
    	       {
                    
                   // cas ou le nombre de places est complet
                 if((fil1.nbpr == fil1.nbp) && (fil2.nbpr == fil2.nbp) && (fil3.nbpr == fil3.nbp))
                  {
                      rep=save_response(rep,adm,0,2);//aucune filiere et complet 
	    	      
	     	      
	          }else
	          {
	    	    // cas ou la moyenne est inferieure à la moyenne de la filiere 
	    	     rep=save_response(rep,adm,0,4);//Negative 
	    	     
	          }
	       }
	  
    	  return rep;
       } //--Traitement de la demande de l'etudiant de type  F---
       else if (adm.typeStudent == F)
       {
          if(adm.fil1 == 1) //fil1 <--> filiere demandé // 1: fil1 des 3 filieres 
          { /*-VU qu'il a choisi fil1, on commence à comparer la moye et les places , dans le   cas ou ce n'est pas faisable d etre orienté vers 
          * cette filiere , le gerant voit la filiere qui lui convient et lui affecte .
           */
              if((adm.moy>= fil1.max) && (fil1.nbp> fil1.nbpr))
              {
                fil1.nbpr++;
                rep=save_response(rep,adm,1,1);//Inscrit 
               
              }else if((adm.moy>=fil2.max) && (fil2.nbp> fil2.nbpr))
              {
                fil2.nbpr++;
                rep=save_response(rep,adm,2,1);//Inscrit
                
                
              }else if((adm.moy>=fil3.max) && (fil3.nbp> fil3.nbpr))
              {
                fil3.nbpr++;
                rep=save_response(rep,adm,3,1);//Inscrit
   
              }else
              {
             	 if(fil2.nbpr == fil2.nbp && fil3.nbpr == fil3.nbp)
             	  {  
             	    
                    rep=save_response(rep,adm,0,2);
                    
	    	  }else
	    	  {
                     rep=save_response(rep,adm,0,4);
                     
	    	  }
	      }
	    
         }else if(adm.fil1 == 2)
	    { //quand il choisis une filiere , on cherche dans les filieres moins que cette filiere 
	    
                if((adm.moy>=fil2.max) && (fil2.nbp > fil2.nbpr))
                {
                   fil2.nbpr++;
                   rep=save_response(rep,adm,2,1);//Inscrit
                   
                   
                }else if((adm.moy>=fil3.max )&&(fil3.nbp> fil3.nbpr))
                {
                   fil3.nbpr=fil3.nbpr+1;
                   rep=save_response(rep,adm,3,1);//Inscrit
                   
                   
                }else
                {
                   if( fil2.nbpr == fil2.nbp && fil3.nbpr == fil3.nbp)
             	      {
             	       rep=save_response(rep,adm,0,2);
	    	      }else
	    	      {
                           rep=save_response(rep,adm,0,4);
                          
	    	      }
	    	}
	    	
             }else //if(adm.fil1 == 3)
              {
                     if((adm.moy>=fil3.max) && (fil3.nbp> fil3.nbpr))
                     {
                          fil3.nbpr++;
                          rep=save_response(rep,adm,3,1);
                       
                     }else
                     {
             	       if(fil3.nbpr == fil3.nbp)
             	       {
             	          
                          rep=save_response(rep,adm,0,2);
                       
	    	       }else
	    	       { 
                            rep=save_response(rep,adm,0,4);
     
	    	       }
	             }
                }
       
        return rep;
        
    }
}

/*------Les 3 processus---------*/

//Etudiant N --> Ils n'exigent pas de filiere 

void etudiantN()
{
   request etudN;
   Response resultN[10]; //OU on va stocker les reponses aux requetes deposés
   int indexN=0;
   FILE* f1 = fopen("FichN.txt", "r");
   
   while(fread(&etudN, sizeof(struct request), 1, f1))
   { 
          //Depot de la requete par l'etudiant de type N dans le tampon Sreq
   	  /* les deux P , au debut ils vont pas etre blockés juqu'au ils seront <1 */
	   //p(nvide)
	   sem_wait(nvide); // initalement nvide=4
	   //p(mutex1)
	   sem_wait(mutex1); //initialement mutex1=1
	   
	   //deposer dans le tampon Sreq
	   
	   shared_mem->SReq[shared_mem->x] = etudN;
	   shared_mem->x = ((shared_mem->x)+1)%4; //taille max est egale à 4 du tampon //incrémenter l'index 
	   
	   //liberer les semaphores 
	   //v(mutex1)
	   sem_post(mutex1);
	   //v(nplein)
	   sem_post(nplein);
	    
    	   if(etudN.reqtype != 0)  //quand c n'est pas terminé
    	   {
    	   
    	       //msgrcv, msgsnd - Opérations sur les files de messages System V
    	        msgrcv(Qrepid,&QRep, sizeof(QRep),N,0); //N=1
                resultN[indexN] = QRep.rep; 
	    	indexN++;
	   }
	  
   }
sleep(5);
printf("\n---Results of student's N Requests ---\n");
for(int i = 0; i<indexN; i++)
{
	printf("Type-reponse:%d | Nom:%d | Type-etudiant: %d | Moyenne: %.2f | Filiere demande: %d | Filiere affecte: %d\n", resultN[i].typerep,resultN[i].nom_res,resultN[i].typeEtudiant,resultN[i].moy_rep,resultN[i].filiere_req,resultN[i].filiere_aff);
	
}
	
fclose(f1);	
exit(0);
}

//----Etudiant F -> Exige une filiere donnée

void etudiantF()
{
   Response resultF[10];
   int indexF=0;
   
   request etudF, desist;

   FILE* f2 = fopen("FichF.txt", "r");
   
    while(fread(&etudF, sizeof(struct request), 1, f2))
    {
	//p(nvide)
	sem_wait(nvide);
	
	//v(mutex1)
        sem_wait(mutex1);
        
	//deposer la requete dans le tampon SReq (Shared memory)
	
	shared_mem->SReq[shared_mem->x]=etudF;
	shared_mem->x = (shared_mem->x+1)%4;
		
	//v(mutex1)
	sem_post(mutex1);
	
	//v(nplein)
	sem_post(nplein);
	
        
    	if(etudF.reqtype != 0) // c'est a dire on a pas encore terminé 
    	{ 
    	   msgrcv(Qrepid,&QRep, sizeof(QRep),F,0);

	   if(QRep.rep.filiere_aff != QRep.rep.filiere_req)
	   { 
		if(rand()%2 == 0) //0: desistement , 1: non 
		{
		    etudF.reqtype = 2; //desistement 
		    etudF.typeEtud= F;
		    etudF.nom_req=QRep.rep.nom_res;
		    etudF.moy=QRep.rep.moy_rep;
		    etudF.filiere=(QRep.rep.filiere_req+1)%4+1;
		    
		    //p(nvide)
		    sem_wait(nvide);
		    
		    //p(mutex1)
		    sem_wait(mutex1);
		    
		    //deposer le desistement 
		    shared_mem->SReq[shared_mem->x]=etudF; 
	   	    shared_mem->x = (shared_mem->x+1)%4;
	   	   
		    //v(mutex1)
		    sem_post(mutex1);
		    //v(nplein)
		    sem_post(nplein);
		   
	           msgrcv(Qrepid,&QRep, sizeof(QRep),F,0);
	           
	   	   //2nd desist 
	           if(QRep.rep.filiere_aff != QRep.rep.filiere_req)
	   	    {	
	         	   if(rand()%2 == 0) // O: desist ; 1: non 
	         	   {
		               etudF.reqtype = 2; //desistemnt 
		               etudF.typeEtud= F; 
		               etudF.nom_req=QRep.rep.nom_res;
		               etudF.moy=QRep.rep.moy_rep;
		               etudF.filiere =(QRep.rep.filiere_req+1)%4+1;
		             
			      //p(nvide)
			      sem_wait(nvide);
			      //p(mutex1)
			      
			     sem_wait(mutex1);
			     shared_mem->SReq[shared_mem->x]=etudF; 
	   		     shared_mem->x= (shared_mem->x+1)%4;
	   		     
			     //v(mutex1)
			     sem_post(mutex1);
			     //v(nplein)
			     sem_post(nplein);
			     msgrcv(Qrepid,&QRep, sizeof(QRep),F,0);
			     
	                  }
	            }
	        }
	}
	   //enregistrer reponse si filier dem
	    resultF[indexF] = QRep.rep; 
	    indexF++;
	
       }
  	   
      }
       
fclose(f2);
sleep(4);
printf("\n---Results of student's F Requests---\n");
for(int i = 0; i<indexF; i++)
{
	printf("Type-reponse:%d | Nom:%d | Type-etudiant: %d | Moyenne: %.2f | Filiere demande : %d | Filiere affecte: %d\n", resultF[i].typerep,resultF[i].nom_res, resultF[i].typeEtudiant,resultF[i].moy_rep, resultF[i].filiere_req,resultF[i].filiere_aff);
	
}
exit(0);	
}

//----Processus Gerant 
void Affect()
{
  /*-- declaration des 3 filieres */
  
  filiere fil1 ={1, 5, 0, 16.0};
  filiere fil2 ={2, 5, 0, 12.5};
  filiere fil3 ={3, 5, 0, 10.0};
  
  Gerant Ad[20]; 
  
  int nb_fich_trait =0;
  request req;
  Response rep;
  int i =0, j=0,index=0;
  
  
  while(nb_fich_trait!= 2) //veut dire que j'ai pas encore traité tout les requetes des 2 fichiers 
  {
            
           
	    //p(nplein)
	    sem_wait(nplein);
	    
	    req = shared_mem->SReq[index]; 
	    index= (index+1)%4; 
	    
	    //v(nvide)
	    sem_post(nvide);	
	    if(req.reqtype == 1) //Inscription 
            {//enregistrer dans table gerant 
            
        	Ad[j].nomE = req.nom_req;
        	Ad[j].typeStudent = req.typeEtud;
        	Ad[j].nbr_desist = 0;
        	Ad[j].moy = req.moy;
        	Ad[j].fil1 = req.filiere;
    			
        	rep = request_treatment(Ad[j],fil1,fil2,fil3);
        	
        	Ad[j].typeresp = rep.typerep;
        	Ad[j].fil2=rep.filiere_aff;
        	
      		//type de l'etudiant pour envoyer le msg
      		if (Ad[j].typeStudent==F) //F
       		{
         		QRep.mesg_type = F;
       		}else
       		{
        	    QRep.mesg_type = N;//N
       		}
       		
       		QRep.rep = rep;
       		msgsnd(Qrepid,&QRep, sizeof(QRep),0); 
       		j++;
    	    }else if(req.reqtype == 2) //desistement
            {
               i=0;
               
               //chercher etudiant dans le tampon 
                while(Ad[i].nomE != req.nom_req)
                {i++;}
                
            	//on incrémente de 1 car il a fait une demande de desist donc le nbr +1
                Ad[i].nbr_desist=Ad[i].nbr_desist+1; 
                
                if(Ad[i].nbr_desist == 2 && req.filiere== Ad[i].fil2)
                 {//nbr desist = 2  <--> envoie reponse exclue
      		   rep.nom_res = Ad[i].nomE;
      		   rep.typeEtudiant = Ad[i].typeStudent;
      		   rep.filiere_req = Ad[i].fil1;
		   rep.typerep = 3; //exclue
		   rep.filiere_aff = 0;
		   Ad[i].typeresp=3; //exclue 
                 }else
                 { 
                   //retraitement de la requete 
      	           rep = request_treatment(Ad[i],fil1,fil2,fil3);
      	         }
      	         
                    QRep.mesg_type = F; //car que l'etudiant F qui va faire un desistement 
                    QRep.rep = rep;
                    msgsnd(Qrepid,&QRep, sizeof(QRep),0); 
           }
            else if(req.reqtype == 0) //terminé
    	    {
    		  /* Quand un gerant termine un fichier on incrémente le nbr pouqu'on soit sure que toutes 
    		  les requetes ont été traité */
    			nb_fich_trait=nb_fich_trait+1;
             }
    		 
  }
  
  //-----Affichage du tableau Gerant (Administrateur)
  printf("\n---Tableau des gerants ---\n");
  for(int y=0; y<j; y++)
  {
  	printf("\ntype-reponse:%d | nom:%d | type-etudiant: %d | Moyenne:%.2f | Filiere demande : %d | Filiere affecte: %d | nbr_desist: %d\n", Ad[y].typeresp,Ad[y].nomE, Ad[y].typeStudent, Ad[y].moy, Ad[y].fil1, Ad[y].fil2 , Ad[y].nbr_desist);
  }
  exit(0);
}



int main()
{
//--Remplissage des fichiers F et N--   
filetudF();
filetudN();

//--initialisation des semaphores--
init_sem();
int id1,id2,id3;
shared_mem->x = 0;

//--Creation des processus--
id1 = fork();
if(id1 == 0)
{
etudiantN();
}
id2 = fork();
if(id2 == 0)
{
etudiantF();
}
id3 = fork();
if(id3 ==0)
{
Affect();
}

wait(0);
wait(0);
wait(0);

shmdt(shared_mem);
shmctl(Sreqid,IPC_RMID,0);
sem_unlink("mutex1");
sem_unlink("nplein");
sem_unlink("nvide");
sem_close(mutex1);
sem_close(nvide);
sem_close(nplein);
msgctl(Qrepid, IPC_RMID,0);

return 0;
}
