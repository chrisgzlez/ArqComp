#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pmmintrin.h>

int main(int argc, char **argv) {

  double    D, L;
  double    *A;
  int       R;

  if (argc < 3) {
    return 1;
  }

  D = atoi(argv[1]);
  L = atoi(argv[2]);

  R = D >= 64 / (sizeof (double)) ? L : L * (8/D);
  printf("D: %.2f L: %.2f R: %d\n", D, L, R);

  A = (double*)_mm_malloc(R*D*sizeof(double), 64);

  double resultado;
  int index[R];

  for (int i=0;i<R;i++){
    index[i] = i*D;
  }

  for (int i=0;i<R;i++) {
    resultado += A[index[i]];
  }

  return resultado;

}
