#include <stdlib.h>
#include <stdio.h>
#define N 10

int main(){

  double	    **a,**b,**d;
  double	    *c,*e;
  int		      *ind;

  double	    f;
  
  *a = (double**)malloc(N * sizeof(double*));
  *d = (double**)malloc(N * sizeof(double*));
  *b = (double**)malloc(8 * sizeof(double*));
  ind = (double*)malloc(N * sizeof(double));
  c = (double*)malloc(8 * sizeof(double));
  e = (double*)malloc(N * sizeof(double));
   
  for(int i=0;i<N;i++){
    a[i] = (double*)malloc(8 * sizeof(double));
    d[i] = (double*)malloc(N * sizeof(double));
    
    if(i<8){
      b[i] = (double*)malloc(N * sizeof(double));
    }  

    // Inicilizar matrix D
    // TODO: check for seffault<

    for(int j = 0; j < N; j++) {
      d[i][j] = 0;
      //Inicializar los valores de la matriz a
      if(j<8){
        
      }
    }

  }


  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      for(int k=0;k<N;k++){
        d[i][j] += 2 * a[i][k] * (b[k][j] - c[k]);
	
      }
    }
  }

  f = 0;

  for(int i=0; i<N;i++){
    e[i] = d[ind[i]][ind[i]]/2;
    f+=e[i];
  }

  printf("Este es el valor de f:%.f",f);



   
}
