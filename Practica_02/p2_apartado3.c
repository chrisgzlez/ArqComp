#include <immintrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define N 10

// Tamaño de cache de L1 48 kiB -- Entran 768 doubles
// Tamaño de cache de L2 1280 KiB
// El bucle for tiene que entrar en la cache de nivel 1
#define BSIZE 15

// BSIZE TIENE QUE SER MODULO DE N

void print_matrix(double* m, int r, int c);
void print_array(double* m, int n);

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



int main() {

    double  *d;             // Matriz inicializada a 0
    double  *a, *b;         // Matrices que almacenan valores aleatorios
    double  *c, *e;         // Arrays
    int     *ind;           // Array desordenado aleatoriamente con valores de fila/columna sin repetirse
    double  f = 0;          // Variable de salida de la suma
    double    t_ck, ck_per_access = 0; // Ciclos totales y por acceso


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

    // L1: 48 KiBytes -- 49152 bytes
    // int: 4 bytes
    // float: 4 bytes
    // double: 8 bytes
    // Tenemos 4 variables int: 4 * 16 = 64 bytes
    // L1: 49152 - 64 = 49088 bytes (libres)
    // 49088 / 8 = 6136 doubles
    // Queremos tener c en memoria cache: 8 doubles
    // 6136 - 8 = 6128 doubles (libres)
    // 17 accesos por iteracion del bucle for (8 a, 8 b, 1 d)
    // 6128 / 17 = 360 iteraciones del bucle for
    // sqrt(360) = 18.97 -> 18
    // TAMAÑO DE BLOQUE 15 (para contar con posibles datos que no tuvimos en cuenta)


    // Print the sizes of int, float and double
    printf("Size of int is %ld bytes\n", sizeof(int));
    printf("Size of float is %ld bytes\n", sizeof(float));
    printf("Size of double is %ld bytes\n", sizeof(double));

    // Operacion de computo de valores de d
    // TODO: Revisar las condiciones de parada

    // Cargamos el vector c ya que es constante
    __m512d c_vec = _mm512_load_pd(c);

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
                    // Version 3: VECTORIZACION
                    // Cargamos contenidos de los arrays a,b,c en vectores
                    // Son de tipo double (64 bits) en cada vector entran 8 doubles
                    __m512d a_vec, b_vec;

                    *(d + i * N + j) += 2 * a[i * 8] * b[N + j] - c[0];


                    *(d + i * N + j) += 2 * a[i * 8 + 1] * b[1 * N + j] - c[1];
                    *(d + i * N + j) += 2 * a[i * 8 + 2] * b[2 * N + j] - c[2];
                    *(d + i * N + j) += 2 * a[i * 8 + 3] * b[3 * N + j] - c[3];
                    *(d + i * N + j) += 2 * a[i * 8 + 4] * b[4 * N + j] - c[4];
                    *(d + i * N + j) += 2 * a[i * 8 + 5] * b[5 * N + j] - c[5];
                    *(d + i * N + j) += 2 * a[i * 8 + 6] * b[6 * N + j] - c[6];
                    *(d + i * N + j) += 2 * a[i * 8 + 7] * b[7 * N + j] - c[7];

                }
            }
        }
    }

    // TODO: Preguntar si desenrollamos con tantos elementos para que ocupen una linea cache
    // Y por consecuente usar _
    // Operacion de reducion de suma y compute de e
    for (int i = 0; i < N - (N%4); i+=4) {
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

    for (int i = 0; i < (N%4); i++) {
        f += *(d + ind[i]*N + ind[i]) / 2;
    }

    /** FIN COMPUTACION **/


    // Obtenemos el numero de ciclos totales que tardaron
    // las 10 reducciones de suma
    t_ck = get_counter();


    // printf("\n-----------Matrix A------------\n");
    // print_matrix(a, N, 8);

    // printf("\n-----------Matrix B------------\n");
    // print_matrix(b, 8, N);
    //
    // printf("\n-----------Matrix D------------\n");
    // print_matrix(d, N, N);


    // printf("\n-----------Array E------------\n");
    // print_array(e, N);

    printf("------------------------------\n");
    printf("REDUCCION DE SUMA F: %.2f\n", f);
    printf("CICLOS DE RELOJ: %.2f\n", t_ck);
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

