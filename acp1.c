#include <stdio.h>
#include <stdlib.h>
#include <pmmintrin.h>

double main(int argc, char **argv) {

  if (argc < 3) return 1;

  double D, L;

  D = argv[1];
  L = argv[2];

  double *A;

  A = (double*)_mm_malloc(R*D*sizeof(double), 64) 	

  double resultado;
  int index[R]

  for (i=0;i<R;i++){
    index[i] = i*D
  }

  for (i=0;i<R;i++) {
    resultado += A[index[i]]
  }

  return resultado

}
