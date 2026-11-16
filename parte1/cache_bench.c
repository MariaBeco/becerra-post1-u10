/*
 * cache_bench.c - Benchmark de la jerarquia de cache (Parte 1, Unidad 10)
 * Autora: Maria Fernanda Becerra Coronel - 1152509 - UFPS
 *
 * Mide la latencia de acceso a memoria con tres patrones:
 *   1. bench_seq:    recorrido secuencial byte a byte (ns/byte)
 *   2. bench_rand:   acceso aleatorio con indices mezclados por Fisher-Yates (ns/acceso)
 *   3. bench_stride: recorrido con salto fijo para exponer conflictos de asociatividad
 *
 * Compilar sin optimizacion para que gcc no elimine los accesos:
 *   gcc -O0 -o cache_bench cache_bench.c && ./cache_bench
 * Un solo experimento: ./cache_bench seq | ./cache_bench rand | ./cache_bench stride
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define REPEAT      100
#define KB          1024UL
#define MB          (1024UL * 1024UL)
#define RAND_GRANDE (4 * MB)   /* desde aqui bench_rand repite menos veces */
#define REPEAT_RAND_GRANDE 10
#define STRIDE_ARRAY (1 * MB)  /* array del experimento de stride */
#define LINEA       64         /* tamano de linea de cache en bytes */

static double ns_entre(struct timespec t0, struct timespec t1) {
    return (t1.tv_sec - t0.tv_sec) * 1e9 + (t1.tv_nsec - t0.tv_nsec);
}

/* Medir tiempo de acceso secuencial a un array de N bytes */
double bench_seq(size_t n_bytes) {
    volatile char *arr = (volatile char *)malloc(n_bytes);
    if (!arr) return -1.0;
    memset((void *)arr, 1, n_bytes);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    for (int r = 0; r < REPEAT; r++)
        for (size_t i = 0; i < n_bytes; i++)
            (void)arr[i];               /* acceso de lectura */

    clock_gettime(CLOCK_MONOTONIC, &t1);
    free((void *)arr);
    return ns_entre(t0, t1) / (REPEAT * (double)n_bytes);   /* ns/byte */
}

/* Acceso aleatorio usando indices pre-mezclados (Fisher-Yates) */
double bench_rand(size_t n_bytes) {
    size_t n = n_bytes / sizeof(int);
    int *arr = (int *)malloc(n * sizeof(int));
    size_t *idx = (size_t *)malloc(n * sizeof(size_t));
    if (!arr || !idx) { free(arr); free(idx); return -1.0; }

    for (size_t i = 0; i < n; i++) { arr[i] = (int)i; idx[i] = i; }
    /* Mezcla Fisher-Yates */
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t)rand() % (i + 1);
        size_t tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp;
    }

    /* Con arrays grandes cada acceso va a RAM: 100 repeticiones de 64 MB
       tardarian minutos, y 10 bastan para un promedio estable. */
    int reps = (n_bytes >= RAND_GRANDE) ? REPEAT_RAND_GRANDE : REPEAT;

    struct timespec t0, t1;
    volatile long sum = 0;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int r = 0; r < reps; r++)
        for (size_t i = 0; i < n; i++)
            sum += arr[idx[i]];
    clock_gettime(CLOCK_MONOTONIC, &t1);

    free(arr); free(idx);
    return ns_entre(t0, t1) / (reps * (double)n);           /* ns/acceso */
}

int main(void) {
    size_t sizes[] = {
        4*KB, 8*KB, 16*KB, 32*KB,     /* region L1 */
        64*KB, 128*KB, 256*KB,        /* region L2 */
        512*KB, 1*MB, 4*MB,           /* region L3 */
        16*MB, 64*MB                  /* region RAM */
    };
    int n = sizeof(sizes) / sizeof(sizes[0]);
    printf("%-10s  %12s  %14s\n", "Tam (KB)", "Seq ns/byte", "Rand ns/acceso");
    for (int i = 0; i < n; i++) {
        double seq = bench_seq(sizes[i]);
        double rnd = bench_rand(sizes[i]);
        printf("%-10zu  %12.3f  %14.3f\n", sizes[i] / KB, seq, rnd);
    }
    return 0;
}
