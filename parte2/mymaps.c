/*
 * mymaps.c - Mapa de memoria virtual de un proceso (Parte 2, Unidad 10)
 * Autora: Maria Fernanda Becerra Coronel - 1152509 - UFPS
 *
 * Imprime direcciones de la pila, del bloque reservado con malloc y de una
 * variable global, y se queda esperando Enter para que /proc/PID/maps se
 * pueda leer mientras el proceso sigue vivo.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_data = 42;          /* segmento .data */
const char *msg = "hola";      /* el puntero va en .data; la cadena, en .rodata */

int main(void) {
    void *heap = malloc(1024 * 1024);  /* 1 MB: glibc lo pide con mmap, no con brk */
    printf("PID: %d\n", getpid());
    printf("stack var addr: %p\n", (void *)&heap);
    printf("heap addr:      %p\n", heap);
    printf("global addr:    %p\n", (void *)&global_data);
    printf("rodata addr:    %p\n", (void *)msg);
    printf("code addr:      %p\n", (void *)main);
    printf("Presione Enter para continuar...\n");
    getchar();
    free(heap);
    return 0;
}
