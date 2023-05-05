#include <immintrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define N 10
#define SEMILLA 19

// Tamaño de cache de L1 48 kiB -- Entran 768 doubles
// Tamaño de cache de L2 1280 KiB
// El bucle for tiene que entrar en la cache de nivel 1
#define BSIZE 15

// BSIZE TIENE QUE SER MODULO DE N

void print_matrix(double* m, int r, int c);
void print_array(double* m, int n);

// Function to reduce a __m512d vector to a double value
double reduce(__m512d input) {

    __m256d vlow  = _mm512_castpd512_pd256(input);
    __m256d vhigh = _mm512_extractf64x4_pd(input,1);

    __m256d suma0 = _mm256_hadd_pd(vlow, vlow);
    __m256d suma1 = _mm256_hadd_pd(vhigh, vhigh);

    return ((double*)&suma0)[0] + ((double*)&suma0)[2] + ((double*)&suma1)[0] + ((double*)&suma1)[2];
}


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
    double  *c;             // Arrays
    int     *ind;           // Array desordenado aleatoriamente con valores de fila/columna sin repetirse
    double  f = 0;          // Variable de salida de la suma
    double    n_ck;         // Ciclos totales y por acceso


    // Inicializamos semilla de random con la hora del sistema
    srand(SEMILLA);

    // Reservamos memoria para los punteros
    // De forma alineada a 32 bytes
    a = (double*)aligned_alloc(64, N * 8 * sizeof(double *));
    d = (double*)aligned_alloc(64, N * N * sizeof(double *));
    b = (double*)aligned_alloc(64, 8 * N * sizeof(double *));
    c = (double*)aligned_alloc(64, 8 * sizeof(double));
    ind = (int*)aligned_alloc(64, N * sizeof(int));

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
    printf("ind: ");
    for (int pi = 0; pi < 8; pi++) {
        printf("%d ", ind[pi]);
    }

    printf("\n\n");

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
    // Y tiene tamaño 8


    __m512d vec_2 = _mm512_set1_pd(2.0);
    __m512d c_vec = _mm512_load_pd(&c[0]);

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

                    // Cargamos 8 doubles en el vector a_vec
                    // Cargamos los datos de la fila i de la matriz a (N x 8)
                    // esto quiere decir que tenemos que cargar
                    // a[0][j], a[1][j], a[2][j], a[3][j], a[4][j], a[5][j], a[6][j], a[7][j]
                    // donde la fila se calcula como: i*8
                    __m512d a_vec = _mm512_load_pd(a + i * 8);

                    // Cargamos 8 doubles en el vector b_vec
                    // Cargamos los datos de la columna j de la matriz b (8 x N)
                    // esto quiere decir que tenemos que cargar
                    // b[0][j], b[1][j], b[2][j], b[3][j], b[4][j], b[5][j], b[6][j], b[7][j]
                    // donde la fila se calcula como: i*N

                    // Calculamos el vector de posiciones de b
                    __m256i b_positions = _mm256_set1_epi32(N);
                    b_positions = _mm256_mullo_epi32(b_positions, _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7));
                    b_positions = _mm256_add_epi32(b_positions, _mm256_set1_epi32(j));


                    __m512d b_vec = _mm512_i32gather_pd(b_positions, b, 8);


                    // Multiplicamos los vectores a_vec y (b_vec - c_vec)
                    __m512d mult = _mm512_mul_pd(a_vec, _mm512_sub_pd(b_vec, c_vec));

                    // Multiplicamos el resultado anterior por 2 y le restamos c_vec
                    __m512d res = _mm512_mul_pd(vec_2, mult);

                    // Reduce sum of vectors
                    *(d + i * N + j) = reduce(res);
                }
            }
        }
    }

    // TODO: Preguntar si desenrollamos con tantos elementos para que ocupen una linea cache
    // Y por consecuente usar _
    // Operacion de reducion de suma y compute de e

    for (int i = 0; i < N; i+=8) {
        // Calculamos la mascara
        int mask_size = N - i < 8 ? N - i : 8;
        int *mask_values = (int*) aligned_alloc(32, sizeof(int) * 8);

        for (int j = 0; j < 8; j++) {
            mask_values[j] = j < mask_size ? -1 : 0;
        }

        // Initialize mask vector
        __m256i mask = _mm256_setr_epi32(
                mask_values[0], mask_values[1], mask_values[2], mask_values[3],
                mask_values[4], mask_values[5], mask_values[6], mask_values[7]
                );

        // Version 2
        // E es un array temporal
        // Lo podemos substituir por un double temporal
        // O prescindir de el
        // Version 1: *(e + i) = *(d + ind[i]*N + ind[i]) / 2;

        // Obtener los indices: ind[i]*N + ind[i]
        // Indice de las columnas de la matriz d
        __m256i ind_col = _mm256_maskload_epi32(ind+i, mask);

        // TODO: IMPRIMER INDICES DE COLUMNAS, ROWS... EL ERROR DE CALCULO ESTA AQUI

        __m256i ind_rows = _mm256_mullo_epi32(ind_col, _mm256_set1_epi32(N));

        __m256i ind_vec = _mm256_add_epi32(ind_rows, ind_col);

        // Cargamos los valores de d en un vector
        __m512d d_vec = _mm512_i32gather_pd(ind_vec, d, 8);


        int * mascara = (int*) &mask;
        int * ind_col_p = (int*) &ind_col;
        int * ind_rows_p = (int*) &ind_rows;
        int * ind_vec_p = (int*) &ind_vec;
        double * d_vec_p = (double*) &d_vec;
        // Dividimos por 2
        //__m512d res = _mm512_div_pd(d_vec, _mm512_set1_pd(2.0));

        f += reduce(d_vec);

        printf("mask_values:");
        for (int pi = 0; pi < 8; pi++) {
            printf("%d ", mask_values[pi]);
        }

        printf("mask: ");
        for (int pi = 0; pi < 8; pi++) {
            printf("%d ", mascara[i]);
        }

        printf("\nind_col: ");
        for (int pi = 0; pi < 8; pi++) {
            printf("%d ", ind_col_p[i]);
        }

        printf("\nind_rows: ");
        for (int pi = 0; pi < 8; pi++) {
            printf("%d ", ind_rows_p[i]);
        }

        printf("\nind_vec: ");
        for (int pi = 0; pi < 8; pi++) {
            printf("%d ", ind_vec_p[i]);
        }

        printf("\nd_vec: ");
        for (int pi = 0; pi < 8; pi++) {
            printf("%.3f ", d_vec_p[i]);
        }
        printf("\nF: %.3f\n\n", f);
    }
    f /= 2;

    /** FIN COMPUTACION **/


    // Obtenemos el numero de ciclos totales que tardaron
    // las 10 reducciones de suma
    n_ck = get_counter();


    printf("\n-----------Matrix A------------\n");
    print_matrix(a, N, 8);
    printf("\n-----------Matrix B------------\n");
    print_matrix(b, 8, N);
    printf("\n-----------Matrix D------------\n");
    print_matrix(d, N, N);

    printf("------------------------------\n");
    printf("REDUCCION DE SUMA F: %.2f\n", f);
    printf("CICLOS DE RELOJ: %.2f\n", n_ck);
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

