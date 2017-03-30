

/// Clase que trabaja como esclavo
class B
{
  private:
     
      /// Tamaño del arreglo de trabajo
      int N;
      /// Puntero al arreglo de trabajo
      int *arr;
   
  public:      
     
      /// Constructor de la clase
      B(void)
      { N = 0;}
     
      /// Recibe el arreglo 
      void recibe(int *a, int n)
      {
         N = n;
         arr= new int[N];
         for (int i=0; i<N;i++)  arr[i] = a[i];
      }
     
      /// Retorna el arreglo
      void retorna(int *a)
      {
         for (int i=0; i<N;i++)  a[i] = arr[i];	     
      }
     
      /// Modifica el arreglo
      void haz(int v)
      {
         for (int i=0; i<N;i++)  arr[i] *= v;     
      }
};
