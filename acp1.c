#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pmmintrin.h>

int main(int argc, char **argv) {
  // Declaracion Variables
  double    D, L, res=0;
  double    *A;
  int       R;

  // Manejo de input
  if (argc < 3) {
    return 1;
  }

  // Inicializamos Variables
  D = atoi(argv[1]);
  L = atoi(argv[2]);
  R = D >= 64 / (sizeof (double)) ? L : L * (8/D);
  A = (double*)_mm_malloc(R*D*sizeof(double), 64);
  

  // Array de Inidces de acceso a A 
  int index[R];
  for (int i = 0;i < R; i++){
    index[i] = i*D;
  }

  for (int i = 0; i < R*D; i++) {
    double temp = rand() % 200 / 100.;
    A[i] += temp < 1.0 ? temp+1 : temp;
  }
  // Sumatorio de los valores de A
  for (int i = 0; i < R; i++) {
    res += A[index[i]];
  }

  
  fprintf(stdout, "%.2f\n", res);
  return 0;
}
