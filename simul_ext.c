#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombre);

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

void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main()
{
	 char *comando[LONGITUD_COMANDO];
	 char *orden[LONGITUD_COMANDO];
	 char *argumento1[LONGITUD_COMANDO];
	 char *argumento2[LONGITUD_COMANDO];
	 
	 int i,j;
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
     
     fent = fopen("particion.bin","r+b");-
     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
     
     
     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
     
     // Buce de tratamiento de comandos
     for (;;){

		 do {
		 printf (">> ");
		 fflush(stdin);
		 fgets(comando, LONGITUD_COMANDO, stdin);
		 } while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);

	     if (strcmp(orden,"dir")==0) {
            Directorio(&directorio,&ext_blq_inodos);
            continue;
            }
         // Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(&memdatos,fent);
         grabardatos = 0;
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         if (strcmp(orden,"salir")==0){
            printf("!Adiós!\n");
            GrabarDatos(&memdatos,fent);
            fclose(fent);
            return 0;
         }

     }
}


// Desarrollo de métodos

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps)
{
    int i;

    printf("Bytemap de inodos:\n");
    for (i = 0; i < MAX_INODOS; i++) {
        printf("%d ", ext_bytemaps->bmap_inodos[i]);
    }
    printf("\n\n");

    printf("Bytemap de bloques:\n");
    for (i = 0; i < 25; i++) {  // Mostramos solo los primeros 25 elementos
        printf("%d ", ext_bytemaps->bmap_bloques[i]);
    }
    printf("\n");
}

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {
    int argc = 0;
    char *token;
    char *comandos_validos[] = {"info", "bytemaps", "dir", "rename", "imprimir", "remove", "copy", "salir"};
    int num_comandos = sizeof(comandos_validos) / sizeof(comandos_validos[0]);

    // Eliminar el salto de línea al final del comando
    strcomando[strcspn(strcomando, "\n")] = 0;

    // Tokenizar el comando
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

    // Verificar si el comando es válido
    int comando_valido = 0;
    for (int i = 0; i < num_comandos; i++) {
        if (strcmp(orden, comandos_validos[i]) == 0) {
            comando_valido = 1;
            break;
        }
    }

    if (!comando_valido) {
        printf("ERROR: Comando ilegal [bytemaps,copy,dir,info,imprimir,rename,remove,salir]\n");
        return -1;
    }

    // Verificar el número correcto de argumentos
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
               strcmp(orden, "dir") == 0 || strcmp(orden, "salir") == 0) {
        if (argc != 1) {
            printf("ERROR: %s no requiere argumentos\n", orden);
            return -1;
        }
    }

    return 0;
}



