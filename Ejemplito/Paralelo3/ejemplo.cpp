

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




#include <stdio.h>
#include <stdlib.h>
#include "A-MPI.hpp"


// Programa Maestro-Esclavo
int main(int argc, char *argv[]) 
{   
   // Variables para MPI
   int MPI_id, MPI_np;   
   MPI::Init(argc,argv);
   MPI_id = MPI::COMM_WORLD.Get_rank();
   MPI_np = MPI::COMM_WORLD.Get_size();

   
	// Revisa que pueda arrancar el esquema M-E
	if (MPI_np < 2) 
   {
      printf("Se necesitan almenos dos procesadores para el esquema M-E\n");
      return 1;
   }
      
   // Numero de tareas a lanzar
   int N = atoi(argv[1]);
   if (N <=1) N = 3;
   
   
   A_MPI a(MPI_id,MPI_np,N);
   a.haz();
	
   
   MPI::Finalize();
   return 0;
}

