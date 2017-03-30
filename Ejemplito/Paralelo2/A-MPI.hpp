#include "Esquema_ME.hpp"
#include "A.hpp"
#include <time.h>


/// Clase para definir el Esquema Maestro-Esclavo 
/** Clase para definir el Esquema Maestro-Esclavo para programar
    en paralelo mediante el paso de mensajes usando MPI, donde 
    el primer procesador (id = 0) es el nodo mestro y el resto 
    son los nodos esclavos. Las tareas se reparten de una a una
    entre los nodos esclavos, si hay mas tareas que procesadores
    se asignaran mas de una tarea a cada esclavo, el reparto no
    necesariamente es homogeneo.
*/
/** @author Antonio Carrillo Ledesma
    @date primavera 2010
    @version 1.0.0 
    @bug No hay errores conocidos
*/class A_MPI: public A, public Esquema_ME
{
  private:
      /// Tiempo inicial
      time_t t1;
      /// Tiempo final  
      time_t t2;
      /// Número de tareas por nodo esclavo
      int nta;
  
  public: 	
      
      // Constructor de la clase
      A_MPI(int id, int np, int n) : Esquema_ME(id, np) , A(n)
      {
         // Controlador del esquema M-E
         if (id == 0) 
         {	
            time(&t1);
            
            printf("\nMaestro ID=%d\n",id);
            fflush(stdout);
            // manda el numero de tareas que deben hacer cada procesador
            generaRepartoCarga(N);

         } else {
            // Numero de tareas a soportar
            MPI::COMM_WORLD.Recv(&nta, 1, MPI::INT, 0, 1);
            printf("\nEsclavo ID=%d  Tareas = %d\n",id,nta);
            fflush(stdout);
            pb = new B[nta];
         }
      }

      // Destructor de la clase
      ~A_MPI()
      {
         if (id == 0) // Maestro
         {	
            time(&t2);
            printf("Tiempo Cálculo: %f\n",difftime(t2,t1));
         }
      }

      /// Funcion principal paralelizada
      void haz(void)
      {
         int i,j;

         if (id == 0)
         {
            int xnp, st, msn[10];

            for (i =0; i < N; i++)
            {
               for (j=0; j<N;j++)  arr[j] = 1;
               reparteCargaTrabajo(xnp, st, i);
               msn[0] = st;
               msn[1] = i;
               msn[2] = N;
               msn[3] = i+1;
               MPI::COMM_WORLD.Send(&msn, 4, MPI::INT, xnp, 1);
               MPI::COMM_WORLD.Send(arr, N, MPI::INT, xnp, 1);
               
               //~ pb[i].recibe(arr,N);
               //~ pb[i].haz(i+1);
               //~ pb[i].retorna(arr);
               MPI::COMM_WORLD.Recv(arr, N, MPI::INT, xnp,1);
               
               
               for (j=0; j<N;j++) printf(" %d",arr[j]);
               printf("\n");
            }
            // Finalizacion de las tareas            
            int sw = 0;
            for (i = 1; i < np; i++) MPI::COMM_WORLD.Send(&sw, 1, MPI::INT, i, 1);
         } else {
            int i, sw = 1;
            int msn[10];

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
      }
};




