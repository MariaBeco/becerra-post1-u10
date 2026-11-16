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

int main(void) {
    size_t sizes[] = {
        4*KB, 8*KB, 16*KB, 32*KB,     /* region L1 */
        64*KB, 128*KB, 256*KB,        /* region L2 */
        512*KB, 1*MB, 4*MB,           /* region L3 */
        16*MB, 64*MB                  /* region RAM */
    };
    int n = sizeof(sizes) / sizeof(sizes[0]);
    printf("%-12s  %10s\n", "Array size", "ns/byte");
    for (int i = 0; i < n; i++) {
        double lat = bench_seq(sizes[i]);
        printf("%-12zu  %10.3f\n", sizes[i] / KB, lat);
    }
    return 0;
}
