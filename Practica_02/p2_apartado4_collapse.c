#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#define SEMILLA 19

// Tamaño de Linea cache 64 KiB

#define BSIZE 15
// BSIZE TIENE QUE SER MODULO DE N


int N = 10;     // Numero de Elementos
int C = 4;      // Numero de Threads


void start_counter();
double get_counter();
double mhz();

/* Initialize the cycle counter */

static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;

/* Set *hi and *lo to the high and low order bits of the cycle counter.
Implementation requires assembly code to use the rdtsc instruction. */
void access_counter(unsigned *hi, unsigned *lo)
{
    asm("rdtsc; movl %%edx,%0; movl %%eax,%1" /* Read cycle counter */
            : "=r"(*hi), "=r"(*lo)                /* and move results to */
            : /* No input */                      /* the two outputs */
            : "%edx", "%eax");
}

/* Record the current value of the cycle counter. */
void start_counter()
{
    access_counter(&cyc_hi, &cyc_lo);
}

/* Return the number of cycles since the last call to start_counter. */
double get_counter()
{
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    /* Get cycle counter */
    access_counter(&ncyc_hi, &ncyc_lo);

    /* Do double precision subtraction */
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double)hi * (1 << 30) * 4 + lo;
    if (result < 0)
    {
        fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}


void print_matrix(double* m, int r, int c);
void print_array(double* m, int n);

int main(int argc, char** argv) {

    double  *d;             // Matriz inicializada a 0
    double  *a, *b;         // Matrices que almacenan valores aleatorios
    double  *c;         // Arrays
    int     *ind;           // Array desordenado aleatoriamente con valores de fila/columna sin repetirse
    double  f = 0;          // Variable de salida de la suma
    double n_ck = 0;        // Numero de ciclos de ejecucion de la funcion

    // Inicializamos semilla de random con la hora del sistema
    srand(SEMILLA);

    // Lectura argumentos de entrada
    if (argc > 1) {
        N = atoi(argv[1]);
        if (argc > 2)
            C = atoi(argv[2]);
    }

    // Reservamos memoria para los punteros
    a = (double*)malloc(N * 8 * sizeof(double));
    d = (double*)malloc(N * N * sizeof(double));
    b = (double*)malloc(8 * N * sizeof(double));
    c = (double*)malloc(8 * sizeof(double));
    ind = (int*)malloc(N * sizeof(int));

    // Reserva de matrices y arrays
    // Inicializacion de matrices y arrays
    for(int i = 0; i < N*8; i++) {
        *(a + i) = rand()%200 / 100.;
        *(b + i) = rand()%200 / 100.;
    }

    for(int i = 0; i < 8; i++) {
        *(c+i) = rand()%200 / 100.;
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
    start_counter();


    /// PARALELIZACION
    // Publicas: a, b, c, d, f y ind
    // Privadas: bi, bj, i, j

    // TODO: Probar con dynamic, guided and auto
    // TODO: Probar con collpase (sin BSIZE fors) vs no-collapse
    #pragma omp parallel for schedule(static, BSIZE) num_threads(C)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // TODO: comparar: guardando i*n + j en varible y sin ella
            // Inicializamos d
            *(d + i * N + j) = 0;

            // Version 2: Desenrollado de bucle k
            *(d + i * N + j) += 2 * a[i * 8 + 0] * (b[0 * N + j] - c[0]);
            *(d + i * N + j) += 2 * a[i * 8 + 1] * (b[1 * N + j] - c[1]);
            *(d + i * N + j) += 2 * a[i * 8 + 2] * (b[2 * N + j] - c[2]);
            *(d + i * N + j) += 2 * a[i * 8 + 3] * (b[3 * N + j] - c[3]);
            *(d + i * N + j) += 2 * a[i * 8 + 4] * (b[4 * N + j] - c[4]);
            *(d + i * N + j) += 2 * a[i * 8 + 5] * (b[5 * N + j] - c[5]);
            *(d + i * N + j) += 2 * a[i * 8 + 6] * (b[6 * N + j] - c[6]);
            *(d + i * N + j) += 2 * a[i * 8 + 7] * (b[7 * N + j] - c[7]);
        }
    }

    // MAIN THREAD
    int end = N - (N%8);

    // PARALELIZACION
    #pragma omp parallel for schedule(static, 1) num_threads(C)
    for (int i = 0; i < end; i+=8) {
        f += *(d + ind[i+0]*N + ind[i+0]) / 2;
        f += *(d + ind[i+1]*N + ind[i+1]) / 2;
        f += *(d + ind[i+2]*N + ind[i+2]) / 2;
        f += *(d + ind[i+3]*N + ind[i+3]) / 2;
        f += *(d + ind[i+4]*N + ind[i+4]) / 2;
        f += *(d + ind[i+5]*N + ind[i+5]) / 2;
        f += *(d + ind[i+6]*N + ind[i+6]) / 2;
        f += *(d + ind[i+7]*N + ind[i+7]) / 2;
    }

    // MAIN THREAD
    for (int i = end; i < N; i++) {
        f += *(d + ind[i]*N + ind[i]) / 2;
    }

    /** FIN COMPUTACION **/
    n_ck = get_counter();

    // Prints debugging
    // printf("\n-----------Matrix A------------\n");
    // print_matrix(a, N, 8);
    // printf("\n-----------Matrix B------------\n");
    // print_matrix(b, 8, N);
//
    // printf("\n-----------Matrix D------------\n");
    // print_matrix(d, N, N);
    // printf("\n-----------Array E------------\n");
    // print_array(e, N);
//
    //printf("------------------------------\n");
    //printf("REDUCCION DE SUMA F: %.2f\n", f);
    //printf("CICLOS DE RELOJ: %.2f\n", n_ck);
//
    // Printf final para coger datos
    fprintf(stdout, "%.2f\n", f);
    fprintf(stdout, "%.2f\n", n_ck);
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

