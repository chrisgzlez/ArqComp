#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pmmintrin.h>
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

double mhz(int verbose, int sleeptime)
{
    double rate;

    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6 * sleeptime);
    if (verbose)
        printf("\n Processor clock rate = %.1f MHz\n", rate);
    return rate;
}


int main(int argc, char **argv) {
  // Declaracion Variables
  int       res=0, results[10];
  double    t_ck, ck_per_access = 0; // Ciclos totales y por acceso
  int       *A;
  int       *index;
  int       D, L;
  int       R;

  // Manejo de input
  if (argc < 3) {
    return 1;
  }

  // Inicializamos Variables
  D = atoi(argv[1]);
  L = atoi(argv[2]);
  R = D >= 64 / (sizeof (int)) ? L : (L-1) * (16/D) + 1;
  A = (int*)_mm_malloc(R*D*sizeof(int), 64);
  

  // Inicializacion del array de Indices de acceso a A 
  index = (int*)_mm_malloc(R*sizeof(int), 64);
  for (int i = 0; i < R; i++){
    index[i] = i*D;
  }

  // Fase de Calentamiento: Inicializar A y cargar datos en cache
  for (int i = 0; i < R*D; i++) {
    int temp = rand() % 3;
    int sign = rand() % 2;
    temp  = temp < 1 ? (temp+1) : temp;
    if (sign) temp *= -1;
    A[i] = temp;
  }

  // Comenzamos medición del tiempo
  start_counter();

  // Repeticion 10 veces de la reduccion de suma
  for (int j = 0; j < 10; j++) {
    // Inicializamos res a 0    
    res = 0; 

    // Reduccion de Suma de los valores de A
    for (int i = 0; i < R; i++) {
      res += A[index[i]];
    }

    // Añadimos resultado al array de resultados
    results[j] = res;
  }
 
  // Obtenemos el numero de ciclos totales que tardaron
  // las 10 reducciones de suma
  t_ck = get_counter();

  // Obtenemos la media de ciclos por acceso 
  // dividiendo entre el numero de iteraciones
  ck_per_access = t_ck / (10.*R);
  
  // Imprimimos el valor de D, L y los ciclos de reloj
  // Para redireccionarlos en el script a un archivo csv
  // Se incluyo la palabra Res para facilitar el filtrado mediante grep
  fprintf(stdout, "Res, %d, %d, %1.10f\n", D, L, ck_per_access);

  // Liberamos memoria de ambos arrays dinamicos
  _mm_free(index);
  _mm_free(A);
  
  // Imprimimos el vector de resultados para 
  // asegurarnos que el compilador no realiza ninguna 
  // optimizacion no deseada
  for (int i = 0; i < 10; i++) {
    printf("%d ", results[i]);
  }
  printf("\n");
  
  return 0;
}
