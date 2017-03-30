

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
  
      /// Comportamiento a paralelizar 0
      virtual void P0(int N)
      {
         pb = new B[N];
      }
      
      /// Comportamiento a paralelizar 1
      virtual void P1(int i, int *arr, int N)
      {
         pb[i].recibe(arr,N);
      }
      
      /// Comportamiento a paralelizar 2
      virtual void P2(int i, int xi)
      {
         pb[i].haz(xi);
      }
      
      /// Comportamiento a paralelizar 3
      virtual void P3(int i, int *arr)
      {
         pb[i].retorna(arr);
      }
  
      
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
         P0(N);
         for (i =0; i < N; i++)
         {
            for (j=0; j<N;j++)  arr[j] = 1;
            P1(i,arr,N);
            P2(i,i+1);
            P3(i,arr);
            for (j=0; j<N;j++) printf(" %d",arr[j]);
            printf("\n");
         }	
      }     
};


