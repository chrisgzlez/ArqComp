#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

    /// CARGANDO VALORES ESCALARES
    // Little endian
    // Initialize vectors
    __m256 pares = _mm256_set_ps(0, 2, 4, 6, 8, 10, 12, 14);
    __m256 impares = _mm256_set_ps(1, 3, 5, 7, 9, 11, 13, 15);

    // Compute
    __m256 result = _mm256_sub_ps(impares, pares);

    // Display
    float* f = (float*)&result;
    for (int i = 0; i <= 8; i++) {
        printf("%f ", f[i]);
    }

    /// CARGANDO VALORES DE MEMORIA
    // LOS DATOS TIENEN QUE ESTAR ALINEADOS EN GRUPOS DE 32 bytes
    // ES DECIR, LA DIRECCION DE MEMORIA TEINE QUE SER DIVISIBLE ENTRE 32

    // Cuando tenemos un numero de elementos que no enacajan, por ejemplo:
    // UN vector de floats (4 bytes) de 9 elementos. 9 no es divisible entre 8 (8 * 4bytes = 32)
    // Por lo que tendriamos que hacer dos operaciones, una con un vector de 8 elementos y otra con un vector de 1 elemento
    // Para eso usamos el maskload que carga los datos de una array de memoria aplicando una mascara

    // Esta mascara es un vector de 8 elementos que tiene un numero negativo (-1) en las posiciones que queremos cargar
    // y uno positvo (0) en las que no

    // int array[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    int * array = (int*) aligned_alloc(32, 67 * sizeof(int));

    for (int i = 0; i < 67; i++) {
        array[i] = i;
    }


    // Initialize mask vector
    __m256i mask = _mm256_setr_epi32(-1, -1, -1, -1, -1, 0, 0, 0);

    // Load according to mask
    __m256i result2 = _mm256_maskload_epi32(array, mask);


    // Display
    int* i9 = (int*)&result2;
    printf("\nresult2: ");
    for (int j = 0; j < 8; j++) {
        printf("%d ", i9[j]);
    }
    printf("\n");


    __m256i a = _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 8);
    __m256i b = _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 8);

    printf("\n");
    // Display a and b
    int* i1 = (int*)&a;
    printf("A: ");
    for (int j = 0; j < 8; j++) {
        printf("%d ", i1[j]);
    }
    printf("\n");

    int* i3 = (int*)&b;
    printf("B: ");
    for (int j = 0; j < 8; j++) {
        printf("%d ", i3[j]);
    }
    printf("\n");

    __m256i result3 = _mm256_mullo_epi32(a, b);

    printf("\n\n");
    int* i2 = (int*)&result3;
    for (int j = 0; j < 8; j++) {
        printf("%d ", i2[j]);
    }


    double *aaa = (double*) aligned_alloc(32, 4 * sizeof(double));

    for (int i = 0; i < 4; i++) {
        aaa[i] = i;
    }

    __m256d aa_vec = _mm256_load_pd(aaa);

    // display
    printf("\nD: ");
    double* d = (double*)&aa_vec;
    for (int i = 0; i < 4; i++) {
        printf("%f ", d[i]);
    }
    printf("\n");


    // Vectores de 8 doubles: 8 * 8 = 64 bytes
    // Tenemos que hacer el align en 32 bytes : Cosa de hacer el align_alloc
    // 32 bytes = 4 doubles

    // Comprobar na iteracion cuantos doubles vamos operar

    //



}