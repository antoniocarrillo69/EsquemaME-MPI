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
    @version 1.1.0 
    @bug No hay errores conocidos
*/
class A_MPI: public A, public Esquema_ME
{
  protected:
     
      /// Tiempo inicial
      time_t t1;
      /// Tiempo final  
      time_t t2;
      /// Número de tareas por nodo esclavo
      int nta;
      /// Número de esclavo en el que estará la tarea
      int xnp; 
      /// Número de tarea dentro del esclavo
      int st;
      /// Arreglo para enviar y/o recibir mensajes
      int msn[10];
   
   
  private:
      int *p;     
  
      /// Sobrecarga de los comportamientos a paralelizar
      void P0(int N)
      {
         pb = new B[N];
      }
      
      /// Sobrecarga de los comportamientos a paralelizar
      void P1(int i, int *arr, int N)
      {
         reparteCargaTrabajo(xnp, st, i);
         p = arr;
         msn[0] = st;
         msn[1] = i;
         msn[2] = N;         
      }
      
      /// Sobrecarga de los comportamientos a paralelizar
      void P2(int i, int xi)
      {
         msn[3] = i+1;
         MPI::COMM_WORLD.Send(&msn, 4, MPI::INT, xnp, 1);
         MPI::COMM_WORLD.Send(p, N, MPI::INT, xnp, 1);
      }
      
      /// Sobrecarga de los comportamientos a paralelizar
      void P3(int i, int *arr)
      {
         //~ pb[i].retorna(arr);
         MPI::COMM_WORLD.Recv(p, N, MPI::INT, xnp,1);
         arr = p;
      }
  
     
  public: 	
      
      /// Constructor de la clase
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
            P0(nta);
         }
      }

      
      /// Destructor de la clase
      ~A_MPI()
      {
         if (id == 0) // Maestro
         {	
            time(&t2);
            printf("\nTiempo Cálculo: %f\n",difftime(t2,t1));
         } 
      }

      
      /// Sobrecarga del la aplicacion maestro
      void haz(void)
      {
         if (id == 0)
         {
            // Lanza la aplicacion
            A::haz();
            
            // Finalizacion de las tareas            
            int sw = 0;
            for (int i = 1; i < np; i++) MPI::COMM_WORLD.Send(&sw, 1, MPI::INT, i, 1);
         } else Esclavo();
      }
      
      
      /// Esclavo
      void Esclavo(void)
      {
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

               A::P1(i,xarr,N);
               A::P2(i,msn[3]);
               A::P3(i,xarr);
               
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
};




