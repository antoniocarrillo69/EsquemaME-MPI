FARMER en C y C++ bajo MPICH 

 
El  presente  proyecto  es  sobre  la  implementación  de  un  Maestro-Esclavo  (farmer)  en  el  lenguaje  de  programación  C  y  C++  bajo  MPICH  trabajando  en  un  cluster  Linux  Debian.  

Donde  tomando  en  cuenta  la  implementación  en estrella  del  cluster  y  el  modelo  de  paralelismo de MPI o MPICH, el nodo maestro tendrá comunicación sólo con cada nodo esclavo y no  existirá  comunicación  entre  los  nodos  esclavos. Esto  reducirá  las  comunicaciones  y  optimizará el paso de  mensajes. 
