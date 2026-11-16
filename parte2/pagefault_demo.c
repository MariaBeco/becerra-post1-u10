/*
 * pagefault_demo.c - Page faults y crecimiento de VmRSS (Parte 2, Unidad 10)
 * Autora: Maria Fernanda Becerra Coronel - 1152509 - UFPS
 *
 * Reserva 100 MB con malloc y muestra VmSize/VmRSS antes y despues de
 * escribir en cada pagina con memset.
 *   gcc -o pagefault_demo pagefault_demo.c && ./pagefault_demo
 *   /usr/bin/time -v ./pagefault_demo
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    size_t N = 100 * 1024 * 1024;  /* 100 MB */
    char *buf = (char *)malloc(N);
    if (!buf) { perror("malloc"); return 1; }

    /* Antes de tocar la memoria: reservada (VmSize) pero no residente (VmRSS) */
    printf("Antes del memset:\n");
    fflush(stdout);
    system("grep -E \"VmPeak|VmSize|VmRSS\" /proc/$PPID/status");

    /* Forzar page faults escribiendo en cada pagina */
    memset(buf, 0xAA, N);

    /* Despues: VmRSS debe haber crecido unos 100 MB */
    printf("Despues del memset:\n");
    fflush(stdout);
    system("grep -E \"VmPeak|VmSize|VmRSS\" /proc/$PPID/status");

    free(buf);
    return 0;
}
