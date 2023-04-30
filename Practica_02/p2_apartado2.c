#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define N 10
#define SEMILLA 19

// Tamaño de Linea cache 64 KiB

#define BSIZE 15
// BSIZE TIENE QUE SER MODULO DE N

void print_matrix(double* m, int r, int c);
void print_array(double* m, int n);

int main() {

    double  *d;             // Matriz inicializada a 0
    double  *a, *b;         // Matrices que almacenan valores aleatorios
    double  *c, *e;         // Arrays
    int     *ind;           // Array desordenado aleatoriamente con valores de fila/columna sin repetirse
    double  f = 0;          // Variable de salida de la suma

    // Inicializamos semilla de random con la hora del sistema
    srand(SEMILLA);

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
    }

    // Creacion del array desordenado de indices
    int aux[N];
    for(int i = 0; i < N; i++) aux[i] = i;

    int z = 0;
    while (z < N) {
        int tmp = rand()%N;
        if (aux[tmp] != -1) {
            ind[z++] = aux[tmp];
            aux[tmp] = -1; 
        }
    }
    
    /** COMPUTACION **/
    
    // Operacion de computo de valores de d
    // TODO: PREGUNTAR JAVI SOBRE BLOCK TILING
    // Y SI TRAER LINEA CACHE
    for(int bi = 0; bi < N; bi += BSIZE) {
        for (int bj = 0; bj < N; bj += BSIZE) {
            // TODO: Calcular min(bi + BSIZE, N) y lo mismo para j
            int min_i = (bi + BSIZE) < N ? (bi + BSIZE) : N;
            int min_j = (bj + BSIZE) < N ? (bj + BSIZE) : N;
            for (int i = 0; i < min_i; i++) {
                for (int j = bj; j < min_j; j++) {
                    // TODO: comparar: guardando i*n + j en varible y sin ella
                    // Inicializamos d
                    *(d + i * N + j) = 0;
                    // Version 2: Intercambio de bucles
                    // Accedemos a los datos de forma lineal
                    // Ya lo haciamos en la Version 1

                    // Version 2: Desenrollado de bucle k
                    *(d + i * N + j) += 2 * a[i * 8 + 0] * (b[0 * N + j] - c[0]);
                    *(d + i * N + j) += 2 * a[i * 8 + 1] * (b[1 * N + j] - c[1]);
                    *(d + i * N + j) += 2 * a[i * 8 + 2] * (b[2 * N + j] - c[2]);
                    *(d + i * N + j) += 2 * a[i * 8 + 3] * (b[3 * N + j] - c[3]);
                    *(d + i * N + j) += 2 * a[i * 8 + 4] * (b[4 * N + j] - c[4]);
                    *(d + i * N + j) += 2 * a[i * 8 + 5] * (b[5 * N + j] - c[5]);
                    *(d + i * N + j) += 2 * a[i * 8 + 6] * (b[6 * N + j] - c[6]);
                    *(d + i * N + j) += 2 * a[i * 8 + 7] * (b[7 * N + j] - c[7]);

                    // Advantages of unroll the loop:
                    // - Less memory access
                    // - More data can be loaded into cache
                    // - Less time to load data
                    // - More data can be processed in one instruction
                    // - Less time to process data
                    // - Less time to jump from loop to loop
                    // - Less time to check loop condition
                    // - Less time to update loop counter
                    // - Less time to update loop bound
                    // - Less time to update loop variable

                    // Version 1: Bucle k
                    // for (int k = 0; k < 8; k++) {
                    //     *(d + i*N + j) += 2 * a[i*8 + k] * b[k*N + j] - c[k];
                    // }
                }
            }
        }
    }

    // TODO: Preguntar si desenrollamos con tantos elementos para que ocupen una linea cache
    // Y por consecuente usar _
    // Operacion de reducion de suma y compute de e
    int end = N - (N%4);
    for (int i = 0; i < end; i+=4) {
        // Version 2
        // E es un array temporal
        // Lo podemos substituir por un double temporal
        // O prescindir de el
        // Version 1: *(e + i) = *(d + ind[i]*N + ind[i]) / 2;

        f += *(d + ind[i]*N + ind[i]) / 2;
        f += *(d + ind[i+1]*N + ind[i+1]) / 2;
        f += *(d + ind[i+2]*N + ind[i+2]) / 2;
        f += *(d + ind[i+3]*N + ind[i+3]) / 2;
    }

    for (int i = end; i < N; i++) {
        f += *(d + ind[i]*N + ind[i]) / 2;
    }

    /** FIN COMPUTACION **/

    printf("\n-----------Matrix A------------\n");
    print_matrix(a, N, 8);
    printf("\n-----------Matrix B------------\n");
    print_matrix(b, 8, N);

    printf("\n-----------Matrix D------------\n");
    print_matrix(d, N, N);
    printf("\n-----------Array E------------\n");
    print_array(e, N);

    printf("------------------------------\n");
    printf("REDUCCION DE SUMA F: %.2f\n", f);
    printf("Count: %d\n", count);
}

void print_matrix(double* m, int r, int c) {
    for(int i = 0; i < r; i++){
        printf("ROW %d: ", i);
        for(int j = 0; j < c; j++) {
            printf("%5.2f ", *(m + i*c + j));
        }
        printf("\n");
    }
}

void print_array(double* a, int n) {
    for(int i = 0; i < n; i++){
        printf("%5.2f ", a[i]);
    }
    printf("\n");
}

