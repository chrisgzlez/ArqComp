#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define N 10

void print_matrix(double* m, int r, int c);
void print_array(double* m, int n);

int main() {

    double  *d;             // Matriz inicializada a 0
    double  *a, *b;         // Matrices que almacenan valores aleatorios
    double  *c, *e;         // Arrays
    int     *ind;           // Array desordenado aleatoriamente con valores de fila/columna sin repetirse
    double  f = 0;          // Variable de salida de la suma

    // Inicializamos semilla de random con la hora del sistema
    srand(time(NULL));
    // Reservamos memoria para los punteros
    a = (double*)malloc(N * 8 * sizeof(double *));
    d = (double*)malloc(N * N * sizeof(double *));
    b = (double*)malloc(8 * N * sizeof(double *));
    c = (double*)malloc(8 * sizeof(double));
    e = (double*)malloc(N * sizeof(double));
    ind = (int*)malloc(N * sizeof(int));
    
    // Reserva de matrices y arrays
    // Inicializacion de matrices y arrays
    for (int i = 0; i < N*N; i++) {
        
        // Con este if ahorramos el uso de otro bucle for
        // para reservar memoria para b
        if (i < 8) {
            // el vector c tiene valores aleatorios
            *(c+i) = rand()%200 / 100.;
        }

        // Inicializamos a y b
        if (i < (N*8)) {
            *(a + i) = rand()%200 / 100.;
            *(b + i) = rand()%200 / 100.;
        }

        // Inicializamos d
        *(d + i) = 0;
    }

    // Creacion del array desordenado de indices
    int aux[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int i = 0;
    while (i < N) {
        int tmp = rand()%N;
        if (aux[tmp] != -1) {
            ind[i++] = aux[tmp];
            aux[tmp] = -1; 
        }
    }
    
    /** COMPUTACION **/
    // Operacion de computo de valores de d
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < 8; k++) {
                *(d + i*N + j) += 2 * a[i*8 + k] * b[k*N + j] - c[k];
            }
        }
    }

    // Operacion de reducion de suma y compute de e
    for (int i = 0; i < N; i++) {
        *(e + i) = *(d + ind[i]*N + ind[i]) / 2;
        f += e[i];
    }

    printf("\n-----------Matrix A------------\n");
    print_matrix(a, N, 8);

    printf("\n-----------Matrix B------------\n");
    print_matrix(b, 8, N);
    
    printf("\n-----------Matrix D------------\n");
    print_matrix(d, N, N);


    printf("\n-----------Array E------------\n");
    print_array(e, N);

    printf("Este es el valor de f:%.f\n", f);
}

void print_matrix(double* m, int r, int c) {
    for(int i = 0; i < r; i++){
        printf("ROW %d:", i);
        for(int j = 0; j < c; j++) {
            printf("%.2f ", *(m + i*c + j));
        }
        printf("\n");
    }
}

void print_array(double* a, int n) {
    for(int i = 0; i < n; i++){
        printf("%.2f ", a[i]);
    }
    printf("\n");
}
