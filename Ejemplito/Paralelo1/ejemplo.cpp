

// Compilar usando
//   mpiCC.mpich -O2 *.cpp -o me -lm

// Iniciar ambiente de ejecucion paralelo
//   lamboot -v

// Correr usando 8 procesadores por ejemplo
//   mpirun.mpich -np 8 me

// Correr usando 4 procesadores segun lista machines.lolb por ejemplo
//   mpirun.mpich -machinefile machines.lolb -np 5 me

// Terminar ambiente de ejecucion paralelo
//   lamhalt -v



#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include "B.hpp"


// Variables para MPI
int MPI_id, MPI_np, npu, *ta;

/// Genera el reparto de trabajo
void generaRepartoCarga(int N)
{
   int i, j;
   
   // Ajusta el número de procesadores a usar   
   if (N >= (MPI_np-1)) npu = MPI_np - 1;
    else npu = N;

   // Arreglo para soportar el reparto de carga por cada procesador
   ta = new int [npu];
   for (i = 0; i < npu; i++) ta[i] = 0;

   // Reparto de carga
   j = 0, i = 0;
   while(i < N)
   {
      ta[j] ++;
      i++; j++;
      if (j >= npu) j = 0;
   }
   
   
   // Visualiza la carga
   for (i=0; i< npu; i++) printf("\nId = %d  ==> TAREAS = %d ",i,ta[i]);
   printf("\n\nNumero de procesadores a usar = %d\n\n",npu);
   
   // Avisa a cada procesador la cantidad de tareas a soportar
   for (i=1; i<= npu; i++)  MPI::COMM_WORLD.Send(&ta[i-1], 1, MPI::INT, i, 1);
   j=0;
   for (i=npu+1; i < MPI_np; i++) MPI::COMM_WORLD.Send(&j, 1, MPI::INT, i, 1);
}



// Programa Maestro-Esclavo
int main(int argc, char *argv[]) 
{
   int i, j, sw, np;
   
   MPI::Init(argc,argv);
   MPI_id = MPI::COMM_WORLD.Get_rank();
   MPI_np = MPI::COMM_WORLD.Get_size();

	// Revisa que pueda arrancar el esquema M-E
	if (MPI_np < 2) 
   {
      printf("Se necesitan almenos dos procesadores para el esquema M-E\n");
      return 1;
   }
   
   // Numero de esclavos a lanzar
   int N = 9;
   
   // Controlador del esquema M-E
   if (MPI_id == 0) // Maestro
   {	
      time_t t1,t2;
      time(&t1);

      int xnp, msn[10];
      printf("\nMaestro ID=%d\n",MPI_id);
      // manda el numero de tareas que deben hacer cada procesador
      generaRepartoCarga(N);
      
      int *arr = new int[N];
      for (i =0; i < N; i++)
      {
         for (j=0; j<N;j++)  arr[j] = 1;

         xnp = (i % npu) + 1;
         msn[0] = i / npu;
         msn[1] = i;
         msn[2] = N;
         msn[3] = i+1;
         MPI::COMM_WORLD.Send(&msn, 4, MPI::INT, xnp, 1);
         MPI::COMM_WORLD.Send(arr, N, MPI::INT, xnp, 1);
         
         // pb[i].recibe(arr,N);
         // pb[i].haz(i+1);
         // pb[i].retorna(arr);
         MPI::COMM_WORLD.Recv(arr, N, MPI::INT, xnp,1);
         
         
         for (j=0; j<N;j++) printf(" %d",arr[j]);
         printf("\n");
      }
      delete []arr;
      
      // Finalizacion de las tareas            
      int sw = 0;
      for (i = 1; i <= npu; i++) MPI::COMM_WORLD.Send(&sw, 1, MPI::INT, i, 1);
      for (i=npu+1; i < MPI_np; i++) MPI::COMM_WORLD.Send(&sw, 1, MPI::INT, i, 1);
      
      time(&t2);
      printf("Tiempo C?lculo: %f\n",difftime(t2,t1));
    } else { // Esclavos
      
      int i, sw = 1, nta = 0;
      int msn[10];
      // Numero de tareas a soportar
      MPI::COMM_WORLD.Recv(&nta, 1, MPI::INT, 0, 1);
      printf("\nEsclavo ID=%d  Tareas = %d\n",MPI_id,nta);
      B *pb = new B[nta];
      while(sw)
      {
         for (i = 0; i < nta; i++)
         {
            MPI::COMM_WORLD.Recv(&msn, 4, MPI::INT, 0, 1);
            N = msn[2];
            //~ printf("\nllego ==> %d %d\n",msn[0],msn[1]);
            int *xarr = new int[N];
            MPI::COMM_WORLD.Recv(xarr, N, MPI::INT, 0, 1);
            //~ for (int j=0; j<N;j++) printf(" %d",xarr[j]);
            
            pb[i].recibe(xarr,N);
            pb[i].haz(msn[3]);
            pb[i].retorna(xarr);
            //~ printf("\n");
            //~ for (int j=0; j<N;j++) printf(" %d",xarr[j]);
            MPI::COMM_WORLD.Send(xarr, N, MPI::INT, 0, 1);
            
            delete []xarr;
         }               
         
         // En espera de mensaje
         MPI::COMM_WORLD.Recv(&sw, 1, MPI::INT, 0,1);
      }
      delete []pb;
            
   }
	
   MPI::Finalize();
   return 0;
}


