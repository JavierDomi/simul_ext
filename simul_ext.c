#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombreantiguo, char *nombrenuevo);

int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
             EXT_DATOS *memdatos, char *nombre);

int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre,  FILE *fich);

int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);

// Comentar las funciones que no se usan o no están implementadas
// void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
// void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
// void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
// void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main()
{
    char comando[LONGITUD_COMANDO];
    char orden[LONGITUD_COMANDO];
    char argumento1[LONGITUD_COMANDO];
    char argumento2[LONGITUD_COMANDO];
    
    int i, j;
    unsigned long int m;
    EXT_SIMPLE_SUPERBLOCK ext_superblock;
    EXT_BYTE_MAPS ext_bytemaps;
    EXT_BLQ_INODOS ext_blq_inodos;
    EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
    EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
    EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
    int entradadir;
    int grabardatos;
    FILE *fent;
    
    // Lectura del fichero completo de una sola vez
    fent = fopen("particion.bin", "r+b");
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
    
    memcpy(&ext_superblock, (EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
    memcpy(&directorio, (EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
    memcpy(&ext_bytemaps, (EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
    memcpy(&ext_blq_inodos, (EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
    memcpy(&memdatos, (EXT_DATOS *)&datosfich[4], MAX_BLOQUES_DATOS * SIZE_BLOQUE);
    
    // Bucle de tratamiento de comandos
    for (;;)
    {
        do {
            printf(">> ");
            fflush(stdin);
            fgets(comando, LONGITUD_COMANDO, stdin);
        } while (ComprobarComando(comando, orden, argumento1, argumento2) != 0);

        if (strcmp(orden, "dir") == 0) {
            Directorio(directorio, &ext_blq_inodos);
            continue;
        }

        // Comentar el código relacionado con las funciones no implementadas
        // Grabarinodosydirectorio(directorio, &ext_blq_inodos, fent);
        // GrabarByteMaps(&ext_bytemaps, fent);
        // GrabarSuperBloque(&ext_superblock, fent);
        // if (grabardatos)
        //     GrabarDatos(memdatos, fent);
        grabardatos = 0;

        // Si el comando es salir se habrán escrito todos los metadatos
        // faltan los datos y cerrar
        if (strcmp(orden, "salir") == 0)
        {
            printf("!Adiós!\n");
            // Comentar GrabarDatos para evitar error
            // GrabarDatos(memdatos, fent);
            fclose(fent);
            return 0;
        }
    }
}

// Implementación de la función Directorio
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos)
{
    int i;
    int inodo;
    // Mostrar encabezado
    printf("Contenido del directorio:\n");
    printf("%-20s %-10s\n", "Nombre del archivo", "Número de Inodo");

    // Recorrer el directorio y mostrar las entradas
    for (i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo != NULL_INODO) {  // Si el inodo no es NULL, es una entrada válida
            inodo = directorio[i].dir_inodo;
            printf("%-20s %-10d\n", directorio[i].dir_nfich, inodo);
        }
    }
}

// Implementación mínima de la función ComprobarComando
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {
    int i = 0;
    strcomando[strcspn(strcomando, "\n")] = 0; // Eliminar salto de línea

    while (i < strlen(strcomando) && !isspace(strcomando[i])) {
        orden[i] = strcomando[i];
        i++;
    }
    orden[i] = '\0';

    while (i < strlen(strcomando) && isspace(strcomando[i])) {
        i++;
    }

    int j = 0;
    while (i < strlen(strcomando) && !isspace(strcomando[i])) {
        argumento1[j] = strcomando[i];
        i++;
        j++;
    }
    argumento1[j] = '\0';

    while (i < strlen(strcomando) && isspace(strcomando[i])) {
        i++;
    }

    if (i < strlen(strcomando)) {
        j = 0;
        while (i < strlen(strcomando) && !isspace(strcomando[i])) {
            argumento2[j] = strcomando[i];
            i++;
            j++;
        }
        argumento2[j] = '\0';
    } else {
        argumento2[0] = '\0';
    }

    return 0;
}
