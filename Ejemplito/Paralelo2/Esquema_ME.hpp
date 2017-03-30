
#include "mpi.h"

/// Clase base para definir el Esquema Maestro-Esclavo
/** Clase base para definir el Esquema Maestro-Esclavo para   
    programar en paralelo mediante el paso de mensajes usando       
    MPI, donde el primer procesador (id = 0) es el nodo mestro    
    y el resto son los nodos esclavos. Las tareas se reparten
    de una a una entre los nodos esclavos, si hay mas tareas
    que procesadores se asignaran mas de una tarea a cada 
    esclavo, el reparto no necesariamente es homogeneo.
*/
/** @author Antonio Carrillo Ledesma
    @date primavera 2010
    @version 1.0.0 
    @bug No hay errores conocidos
*/
class Esquema_ME
{
   
  protected:
    
      /// Identificador
      int id;   
      /// Número de procesadores
      int np;
      /// Número de tareas por nodo esclavo
      int *ta;
      /// Número de nodos esclavos a utilizar (los que tienen carga)
      int npu;  
  
  
  public: 	
	
      /// Constructor de la clase
      /** @param id Identificador 
          @param np Número de procesadores */
      Esquema_ME(int id, int np)
      {
         this->id = id;
         this->np = np;
         ta = NULL;
      }
      
      /// Destructor de la clase
      ~Esquema_ME()
      {
         if(ta) delete []ta;
      }  
      
      /// Genera el reparto de carga
      /** @param n Número de trabajos */
      void generaRepartoCarga(int n)
      {
         int i, j;
         
         // Ajusta el número de procesadores a usar   
         if (n >= (np-1)) npu = np - 1;
          else npu = n;

         // Arreglo para soportar el reparto de carga por cada esclavo
         ta = new int [npu];
         for (i = 0; i < npu; i++) ta[i] = 0;

         // Reparto de carga
         j = 0, i = 0;
         while(i < n)
         {
            ta[j] ++;
            i++; j++;
            if (j >= npu) j = 0;
         }
         
         // Visualiza la carga
         for (i = 0; i < npu; i++) printf("\nId = %d  ==> TAREAS = %d ",i+1,ta[i]);
         printf("\n\nNumero de procesadores esclavos a usar = %d\n\n",npu);
         
         // Avisa a cada esclavo la cantidad de tareas a soportar
         for (i = 1; i<= npu; i++)  MPI::COMM_WORLD.Send(&ta[i-1], 1, MPI::INT, i, 1);
         j=0;
         for (i = (npu + 1); i < np; i++) MPI::COMM_WORLD.Send(&j, 1, MPI::INT, i, 1);
      }

      
      /// Reparte la carga de trabajo entre los nodos esclavos
      /** @param np Número de esclavo en el que estará la tarea 
          @param st Número de tarea dentro del esclavo
          @param tarea Número de tarea */
      void reparteCargaTrabajo(int &np, int &st, int tarea)
      {
         // Indica el número de esclavo en el que estará la tarea 
         np = (tarea % npu) + 1;
         // Indica el número de tarea dentro del esclavo
         st = tarea / npu;
      }      
      

};




