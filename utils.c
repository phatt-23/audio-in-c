#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* strreverse(char* str) {
    if (!str) return NULL;
    int i = 0, j = strlen(str) - 1;
    while (i < j) {
        char c = str[i];
        str[i++] = str[j];
        str[j--] = c;
    }
    return str;
}
char* note_str_format(char* note) {
    int i = 0, j = 0;
    static char r_str[3] = { 0 };
    while (note[i] != '\0') { r_str[i] = note[i]; i++; };
    for (j = i;j < (int)strlen(r_str);j++) r_str[j] = ' ';
    return r_str;
}
void* my_realloc(void* buffer, int size_type, int* buffer_size, int new_size) {
    void* res = calloc(new_size, size_type);
    if(res == NULL) {
        printf(" ! <mem> buffer realloc - FAIL\n");
        printf(" ! <mem> free memory and exit\n");
        free(buffer);
        free(res);
        exit(1);
    } else {
        printf("\t<mem> buffer realloc - SUCCESS\n");
        printf("\t<mem> adress <old pointer> %p ↓↓\n", buffer);
        printf("\t<mem> adress <new pointer> %p ←←\n", res);
        memcpy(res, buffer, *buffer_size * size_type);
        *buffer_size = new_size;
        free(buffer);
    } 
    return res;
}
void colour_print(int expr, char* colour1, char* colour2, char* str1, double val, char* str2) {
    if (expr) printf(colour1);
    else printf(colour2);
    printf(str1);
    printf("%.2lf", val);
    printf(str2);
    printf("\x1b[0m");
    fflush(stdout);
    putc('\n', stdout);
}