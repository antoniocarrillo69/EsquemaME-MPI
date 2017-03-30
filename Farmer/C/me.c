// Programa Maestro - Esclavo
// Autor Antonio Carrillo Ledesma
// http://www.mmc.igeofcu.unam.mx/acl


// Compilar usando
//   mpicc me.c -o me -lm

// Correr usando 8 procesadores por ejemplo
//   mpirun -np 8 me




#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "/usr/lib/mpich/include/mpi.h"
#include <time.h>

// Funcion de Arnold
double F(double T,double A,double B);
//  Calcula  la  resonancia  de  algun  punto del espacio  de parametros
int Resonancias(int MaxPer,int Ciclos,int Trans,double A,double B, double Eps,int *Pr,int *Qr);



//#define VIS


// Programa Maestro-Esclavo
int main(int argc, char *argv[]) 
{
   int ME_id,MP_np;
   int ME_P, ME_L, ME_sw;
   MPI_Status ME_St;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&ME_id);
	MPI_Comm_size(MPI_COMM_WORLD,&MP_np);
	// Revisa que pueda arrancar el esquema M-E
	if (MP_np < 2) 
   {
      printf("Se necesitan almenos dos procesadores para el esquema M-E\n");
      return 1;
   }
   
   
   // Controlador del esquema M-E
	if (ME_id == 0) 
   {	
      // Maestro
      printf("Maestro-Esclavo, Numero de Esclavos %d\n",MP_np-1);

      ME_P = 1;
		ME_sw = 1;
      time_t inicio,final;
      inicio=time(NULL);
		

		double T;
      for (T = 0.0; T < 1.0; T+=0.0009765625) 
      {
         // Llena de trabajo a los trabajadores 
    		if (ME_P < MP_np) 
         {
            // Aviso de envio de una nueva tarea al nodo P
            MPI_Send(&ME_sw, 1, MPI_INT, ME_P,1, MPI_COMM_WORLD);
            // Preparacion para envio de una nueva tarea

            // Envio de una nueva tarea al nodo P
            MPI_Send(&T, 1, MPI_DOUBLE, ME_P,0, MPI_COMM_WORLD);
#ifdef VIS
            printf("Maestro envio S %d\n",ME_P);
#endif
            ME_P++;
         } else {
            // Recibo de terminación de tarea del nodo L
            MPI_Recv(&ME_L, 1, MPI_INT, MPI_ANY_SOURCE,0, MPI_COMM_WORLD, &ME_St);
#ifdef VIS
            printf("Tarea Terminada S%d\n",ME_L);
#endif

            // Aviso de envio de una nueva tarea al nodo L
            MPI_Send(&ME_sw, 1, MPI_INT, ME_L,1, MPI_COMM_WORLD);
            // Preparacion para envio de una nueva tarea
            
            // Envio de una nueva tarea al nodo L
            MPI_Send(&T, 1, MPI_DOUBLE, ME_L,0, MPI_COMM_WORLD);        
#ifdef VIS
            printf("Maestro envio S %d\n",ME_L);
#endif    
         }
      }
      ME_P--;
#ifdef VIS
      printf("Tareas Pendientes %d\n",ME_P);
#endif
      while(ME_P> 0) 
      {
          // Recibo de terminación de tarea del nodo L     
          MPI_Recv(&ME_L, 1, MPI_INT, MPI_ANY_SOURCE,0, MPI_COMM_WORLD, &ME_St);
#ifdef VIS
          printf("Tarea Terminada S%d\n",ME_L);
#endif
          ME_P--;
#ifdef VIS
          printf("Tareas Pendientes %d\n",ME_P);
#endif
      }
      ME_sw = 0;
      // Aviso de terminación de tareas al nodo P
      for (ME_P = 1; ME_P < MP_np; ME_P++) MPI_Send(&ME_sw, 1, MPI_INT, ME_P,1, MPI_COMM_WORLD);
	   final=time(NULL);
	   printf("El tiempo empleado fue %lf segundos\n",difftime(final,inicio));

	} else {
      // Esclavos

      // Recolector de trabajo    
      FILE* datos;
      char xcad[100];
     	sprintf(xcad,"DatS%d.dat",ME_id);
	   datos = fopen(xcad,"wt");
 
      
      int sw;
		while (1) 
      {
		   double A;
	      // Recibo aviso de envio de una nueva tarea 
	      MPI_Recv(&sw, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &ME_St);
	      if (!sw) 
         {
#ifdef VIS
            printf("Termina esclavo: %d\n",ME_id);
#endif
            break;
         }
         // Recibo una nueva tarea
         MPI_Recv(&A, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &ME_St);
#ifdef VIS
         printf("Esclavo: %d ---> tarea: %lf\n",ME_id,A);
#endif
         // Procesamiento de la tarea
         
         
	      int Pr,Qr;             
	      double B;
         for (B = 0.0; B < 1.0; B+=0.0009765625) 
         {
            if (Resonancias(100, 100, 50, A, B, 0.1,&Pr,&Qr) ) fprintf(datos,"%lf %lf %d %d\n",A,B,Pr,Qr);
         }
         
         // Avisa la terminacion del trabajo
         MPI_Send(&ME_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
      fclose(datos);	
	}
	
	MPI_Finalize();
	return 0;
}


// Funcion de Arnold
double F(double T,double A,double B)
{
	return(T + A + B*sinl(2.0*M_PI*T));
}

//  Calcula  la  resonancia  de  algun  punto del espacio  de parametros
int Resonancias(int MaxPer,int Ciclos,int Trans,double A,double B, double Eps,int *Pr,int *Qr)
{
/***************************************************************************
 * Resonancias  calcula  la  resonancia  de  algun  punto del espacio  de  *
 * parametros                                                              *
 *                                                                         *
 * Entradas  :                                                             *
 *   MaxPer  : Maximo periodo para buscar                                  *
 *   Ciclos  : Numero de ciclos en la busqueda                             *
 *   Trans   : Longitud del transitorio                                    *
 *   A       : Valor del parametro A                                       *
 *   B       : Valor del parametro B                                       *
 *   Eps     : Tolerancia deseada para considerar que una orbita es cerrada*
 *                                                                         *
 * Salidas   : La  funcion  retorna  un  valor  verdadero  si  fue  posible*
 *            calcular la resonancia en algun punto, y en Pr y  Qr retorna *
 *            el numero de vueltas y el  periodo, respectivamente,  de  la *
 *            resonancia.                                                  *
 **************************************************************************/
  int Found=0;            /*** Variable auxiliar para determinar si se encontro alguna resonancia ***/
  int I=0;                /*** Contador de iteraciones ***/
  int J;                  /*** Contador de iteraciones ***/
  double aux;        /*** Variable auxiliar para obtener partes fraccionarias ***/
  double aux2;       /*** Variable auxiliar para obtener partes fraccionarias ***/
  double X;          /*** Auxiliar para almacenar las iteraciones de la funcion de disparos ***/
  double X0;         /*** Almacena el primer valor de las iteraciones para buscar las resonancias ***/
  int K;                  /*** Contador de ciclos ***/

  X0=random()/2.e09;   // Condicion inicial aleatoria
  do {                        // Transitorios
	 X0=modf(F(X0,A,B),&aux);
	 I++;
  } while (I<Trans);
  K=0;
  X=X0;
  do {       // Comienza otro ciclo
	 do {     // Busca una primera aproximacion
		I=0;
		while ((!Found) && (I<MaxPer))   // busca la resonancia
		  {                                       // y halla una primera orbita
			 X=modf(X,&aux); 							// periodica
			 X=F(X,A,B);
			 Found=((fabs(X0-modf(X,&aux))<Eps) || (fabs(fabs(X0-modf(X,&aux))-1.0)<Eps));
			 I++;     // Almacena el periodo de la orbita en la variable I
		  }
		if ((I>=MaxPer) && (!Found)) {
		  K++;
		  X0=X;   //Transitorio variable si no encontro una orbita periodica
		}
	 } while ((!Found) && (K<Ciclos));
	 if (Found) { // Comienza comprobacion de la resonancia hallada
		Found=0;
		if ((I>0) && (I<MaxPer)) {
		  X=modf(X,&aux);
		  X0=X;
		  aux=0.0;
		  for (J=0; J<I; J++) {                 //Ciclo de check
			 X=modf(X,&aux);
			 X=F(X,A,B)+aux;
		  }
		  Found=((fabs(X0-modf(X,&aux2))<=Eps) || (fabs(fabsl(X0-modf(X,&aux2))-1.0)<=Eps));
		  if (Found) { // Se encontro resonancia y paso el check
			 *Pr=aux;               // Asigna numero de vueltas (envolvencia).
			 *Qr=I;                 // Asigna periodo almacenado (resonancia).
			 return(Found);
		  }
		}
	 }
	 X0=modf(X,&aux);         // Si no detecto periodicidad toma otro punto
	 X=X0;                     // mas avanzado de una probable orbita y lo
										// utiliza al volver a entrar al loop.
	 K++;
  } while ((!Found) && (K<Ciclos) && (I<MaxPer) && (J<MaxPer));

  if (!Found)   {            // No encontro resonancia en el punto
	 *Pr=-1;
	 *Qr=-1;
  }
  return(Found);
}
