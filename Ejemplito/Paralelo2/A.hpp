

#include "B.hpp"
#include <stdio.h>




/// Clase que trabaja como maestro
class A
{
  protected:
     
      /// Tamaño del arreglo de trabajo
      int  N;
      /// Puntero a objetos de la clase B
      B   *pb;
      /// Puntero al arreglo de trabajo
      int *arr;
  
  
  public: 	
  
      /// Constructor de la clase
      A(int n)
      {
         N = n;
         arr = new int[N];
      }

      /// Implementacion de esquema maestro-esclavo
      void haz(void)
      {
         int i,j;
         pb = new B[N];
         for (i =0; i < N; i++)
         {
            for (j=0; j<N;j++)  arr[j] = 1;
            pb[i].recibe(arr,N);
            pb[i].haz(i+1);
            pb[i].retorna(arr);
            for (j=0; j<N;j++) printf(" %d",arr[j]);
            printf("\n");
         }	
      }     
};


