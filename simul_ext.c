
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include <stdlib.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre, FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);
void LimpiarPantalla();

int main()
{

    LimpiarPantalla();

    char comando[LONGITUD_COMANDO];
    char orden[LONGITUD_COMANDO];
    char argumento1[LONGITUD_COMANDO];
    char argumento2[LONGITUD_COMANDO];
    
    EXT_SIMPLE_SUPERBLOCK ext_superblock;
    EXT_BYTE_MAPS ext_bytemaps;
    EXT_BLQ_INODOS ext_blq_inodos;
    EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
    EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
    EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
    int grabardatos = 0;
    FILE *fent;
    
    fent = fopen("particion.bin","r+b");
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
    
    memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
    memcpy(directorio,(EXT_ENTRADA_DIR *)&datosfich[3], sizeof(EXT_ENTRADA_DIR) * MAX_FICHEROS);
    memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
    memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
    memcpy(memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
    
    for (;;){
        do {
            printf (">> ");
            fflush(stdin);
            fgets(comando, LONGITUD_COMANDO, stdin);
        } while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);
        
        if (strcmp(orden,"dir")==0) {
            Directorio(directorio,&ext_blq_inodos);
            continue;
        }
        else if (strcmp(orden, "info") == 0) {
            LeeSuperBloque(&ext_superblock);
        } 
        else if (strcmp(orden, "bytemaps") == 0) {
            Printbytemaps(&ext_bytemaps);
        }
        else if (strcmp(orden, "clear") == 0) {
            LimpiarPantalla();
        }
        
        Grabarinodosydirectorio(directorio,&ext_blq_inodos,fent);
        GrabarByteMaps(&ext_bytemaps,fent);
        GrabarSuperBloque(&ext_superblock,fent);
        if (grabardatos)
            GrabarDatos(memdatos,fent);
        grabardatos = 0;
        
        if (strcmp(orden,"salir")==0){
            printf("!Adiós!\n");
            GrabarDatos(memdatos,fent);
            fclose(fent);
            return 0;
        }
    }
}

void LimpiarPantalla()
{
    system("clear");
}

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps)
{
    int i;
    printf("Bytemap de inodos:\n");
    for (i = 0; i < MAX_INODOS; i++) {
        printf("%d ", ext_bytemaps->bmap_inodos[i]);
    }
    printf("\n\n");
    printf("Bytemap de bloques:\n");
    for (i = 0; i < 25; i++) {
        printf("%d ", ext_bytemaps->bmap_bloques[i]);
    }
    printf("\n");
}

void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup)
{
    printf("Información del Superbloque:\n");
    printf("Tamaño del bloque: %d bytes\n", psup->s_block_size);
    printf("Número total de inodos: %d\n", psup->s_inodes_count);
    printf("Número de inodos libres: %d\n", psup->s_free_inodes_count);
    printf("Número total de bloques: %d\n", psup->s_blocks_count);
    printf("Número de bloques libres: %d\n", psup->s_free_blocks_count);
    printf("Primer bloque de datos: %d\n", psup->s_first_data_block);
}

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {
    int argc = 0;
    char *token;
    char *comandos_validos[] = {"info", "bytemaps", "dir", "rename", "imprimir", "remove", "copy", "salir", "clear"};
    int num_comandos = sizeof(comandos_validos) / sizeof(comandos_validos[0]);

    strcomando[strcspn(strcomando, "\n")] = 0;
    token = strtok(strcomando, " ");
    if (token != NULL) {
        strcpy(orden, token);
        argc++;
        token = strtok(NULL, " ");
        if (token != NULL) {
            strcpy(argumento1, token);
            argc++;
            token = strtok(NULL, " ");
            if (token != NULL) {
                strcpy(argumento2, token);
                argc++;
            }
        }
    }

    int comando_valido = 0;
    for (int i = 0; i < num_comandos; i++) {
        if (strcmp(orden, comandos_validos[i]) == 0) {
            comando_valido = 1;
            break;
        }
    }

    if (!comando_valido) {
        printf("ERROR: Comando ilegal [bytemaps,copy,dir,info,imprimir,rename,remove,salir,clear]\n");
        return -1;
    }

    if (strcmp(orden, "rename") == 0 || strcmp(orden, "copy") == 0) {
        if (argc != 3) {
            printf("ERROR: Número incorrecto de argumentos para %s\n", orden);
            return -1;
        }
    } else if (strcmp(orden, "imprimir") == 0 || strcmp(orden, "remove") == 0) {
        if (argc != 2) {
            printf("ERROR: Número incorrecto de argumentos para %s\n", orden);
            return -1;
        }
    } else if (strcmp(orden, "info") == 0 || strcmp(orden, "bytemaps") == 0 || 
         strcmp(orden, "dir") == 0 || strcmp(orden, "salir") == 0 || strcmp(orden, "clear") == 0)
{
        if (argc != 1) {
            printf("ERROR: %s no requiere argumentos\n", orden);
            return -1;
        }
    }

    return 0;
}

int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre) {
    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (strcmp(directorio[i].dir_nfich, nombre) == 0) {
            return directorio[i].dir_inodo;
        }
    }
    return NULL_INODO;
}

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos)
{
    int i;
    unsigned int inodo, tamanio;
    
    // Mostrar encabezado
    printf("Contenido del directorio:\n");
    printf("%-17s %-10s %-10s\n", "Nombre", "Inodo", "Tamaño");
    printf("-----------------------------------\n");

    // Recorrer el directorio y mostrar las entradas
    for (i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo != NULL_INODO) {  // Si el inodo no es NULL, es una entrada válida
            inodo = directorio[i].dir_inodo;
            tamanio = inodos->blq_inodos[inodo].size_fichero;
            printf("%-17s %-10d %-10d\n", directorio[i].dir_nfich, inodo, tamanio);
        }
    }
}


int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo) {
    // Implementación pendiente
    return 0;
}

int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {
    // Implementación pendiente
    return 0;
}

int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre, FILE *fich) {
    int inodo = BuscaFich(directorio, inodos, nombre);
    if (inodo == NULL_INODO) {
        printf("Error: El archivo %s no existe.\n", nombre);
        return -1;
    }

    // Liberar el inodo
    ext_bytemaps->bmap_inodos[inodo] = 0;
    ext_superblock->s_free_inodes_count++;

    // Liberar los bloques de datos
    for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
        unsigned short int bloque = inodos->blq_inodos[inodo].i_nbloque[i];
        if (bloque != NULL_BLOQUE) {
            ext_bytemaps->bmap_bloques[bloque] = 0;
            ext_superblock->s_free_blocks_count++;
            inodos->blq_inodos[inodo].i_nbloque[i] = NULL_BLOQUE;
        }
    }

    // Eliminar la entrada del directorio
    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (strcmp(directorio[i].dir_nfich, nombre) == 0) {
            directorio[i].dir_nfich[0] = '\0';
            directorio[i].dir_inodo = NULL_INODO;
            break;
        }
    }

    // Actualizar el tamaño del archivo a 0
    inodos->blq_inodos[inodo].size_fichero = 0;

    printf("Archivo %s borrado con éxito.\n", nombre);
    return 0;
}


int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich) {
    // Implementación pendiente
    return 0;
}

void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich) {
    // Implementación pendiente
}

void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich) {
    // Implementación pendiente
}

void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich) {
    // Implementación pendiente
}

void GrabarDatos(EXT_DATOS *memdatos, FILE *fich) {
    // Implementación pendiente
}
