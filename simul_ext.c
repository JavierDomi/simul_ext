#include <stdio.h>
#include <string.h>
#include <ctype.h>
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
//int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre, FILE *fich);
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
       } else if (strcmp(orden, "info") == 0) {
           LeeSuperBloque(&ext_superblock);
       } else if (strcmp(orden, "bytemaps") == 0) {
           Printbytemaps(&ext_bytemaps);
       } else if (strcmp(orden, "clear") == 0) {
           LimpiarPantalla();
       } else if (strcmp(orden, "imprimir") == 0) {
           Imprimir(directorio, &ext_blq_inodos, memdatos, argumento1);
       } else if (strcmp(orden, "remove") == 0) {
           Borrar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, fent);
           grabardatos = 1;
       } else if (strcmp(orden, "copy") == 0) {
            Copiar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2, fent);
                grabardatos = 1;
       } else if (strcmp(orden, "rename") == 0) {
            Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);
                grabardatos = 1;
       }

       if (grabardatos) {
           Grabarinodosydirectorio(directorio,&ext_blq_inodos,fent);
           GrabarByteMaps(&ext_bytemaps,fent);
           GrabarSuperBloque(&ext_superblock,fent);
           GrabarDatos(memdatos,fent);
           grabardatos = 0;
       }

       if (strcmp(orden,"salir")==0){
           printf("¡Adiós!\n");
           fclose(fent);
           return 0;
       }
   }
}


void LimpiarPantalla() {
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


int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2)
{
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
              strcmp(orden, "dir") == 0 || strcmp(orden, "salir") == 0 || strcmp(orden, "clear") == 0) {
       if (argc != 1) {
           printf("ERROR: %s no requiere argumentos\n", orden);
           return -1;
       }
   }


   return 0;
}


int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre)
{
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
   printf("Contenido del directorio:\n");
   printf("%-17s %-10s %-10s\n", "Nombre", "Inodo", "Tamaño");
   printf("-----------------------------------\n");
   for (i = 0; i < MAX_FICHEROS; i++) {
       if (directorio[i].dir_inodo != NULL_INODO) {
           inodo = directorio[i].dir_inodo;
           tamanio = inodos->blq_inodos[inodo].size_fichero;
           printf("%-17s %-10d %-10d\n", directorio[i].dir_nfich, inodo, tamanio);
       }
   }
}


int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {
    int inodo = BuscaFich(directorio, inodos, nombre);
    if (inodo == NULL_INODO) {
        printf("Error: El archivo %s no existe.\n", nombre);
        return -1;
    }
    printf("Contenido del archivo %s:\n", nombre);
    unsigned int bytesLeidos = 0;
    unsigned int tamanoArchivo = inodos->blq_inodos[inodo].size_fichero;
    for (int i = 0; i < MAX_NUMS_BLOQUE_INODO && bytesLeidos < tamanoArchivo; i++) {
        unsigned short int bloque = inodos->blq_inodos[inodo].i_nbloque[i];
        if (bloque != NULL_BLOQUE) {
            unsigned int bytesALeer = (tamanoArchivo - bytesLeidos < SIZE_BLOQUE) ? (tamanoArchivo - bytesLeidos) : SIZE_BLOQUE;
            for (unsigned int j = 0; j < bytesALeer; j++) {
                unsigned char c = memdatos[bloque].dato[j];
                if (isprint(c) || c == '\n' || c == '\t' || c == '\r') {
                    putchar(c);
                } else {
                    putchar('.');
                }
            }
            bytesLeidos += bytesALeer;
        }
    }
    printf("\n");
    return 0;
}






int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre, FILE *fich)
{
   int inodo = BuscaFich(directorio, inodos, nombre);
   if (inodo == NULL_INODO) {
       printf("Error: El archivo %s no existe.\n", nombre);
       return -1;
   }
   ext_bytemaps->bmap_inodos[inodo] = 0;
   ext_superblock->s_free_inodes_count++;
   for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
       unsigned short int bloque = inodos->blq_inodos[inodo].i_nbloque[i];
       if (bloque != NULL_BLOQUE) {
           ext_bytemaps->bmap_bloques[bloque] = 0;
           ext_superblock->s_free_blocks_count++;
           inodos->blq_inodos[inodo].i_nbloque[i] = NULL_BLOQUE;
       }
   }
   for (int i = 0; i < MAX_FICHEROS; i++) {
       if (strcmp(directorio[i].dir_nfich, nombre) == 0) {
           directorio[i].dir_nfich[0] = '\0';
           directorio[i].dir_inodo = NULL_INODO;
           break;
       }
   }
   inodos->blq_inodos[inodo].size_fichero = 0;
   printf("Archivo %s borrado con éxito.\n", nombre);
   return 0;
}


void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich)
{
   fseek(fich, 2 * SIZE_BLOQUE, SEEK_SET);
   fwrite(inodos, SIZE_BLOQUE, 1, fich);
   fwrite(directorio, SIZE_BLOQUE, 1, fich);
}


void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich)
{
   fseek(fich, SIZE_BLOQUE, SEEK_SET);
   fwrite(ext_bytemaps, SIZE_BLOQUE, 1, fich);
}


void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich)
{
   fseek(fich, 0, SEEK_SET);
   fwrite(ext_superblock, SIZE_BLOQUE, 1, fich);
}


void GrabarDatos(EXT_DATOS *memdatos, FILE *fich)
{
   fseek(fich, 4 * SIZE_BLOQUE, SEEK_SET);
   fwrite(memdatos, sizeof(EXT_DATOS), MAX_BLOQUES_DATOS, fich);
}

int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich) {
    int inodo_origen = BuscaFich(directorio, inodos, nombreorigen);
    if (inodo_origen == NULL_INODO) {
        printf("Error: El archivo origen %s no existe.\n", nombreorigen);
        return -1;
    }
    if (BuscaFich(directorio, inodos, nombredestino) != NULL_INODO) {
        printf("Error: El archivo destino %s ya existe.\n", nombredestino);
        return -1;
    }
    int inodo_destino = -1;
    for (int i = 0; i < MAX_INODOS; i++) {
        if (ext_bytemaps->bmap_inodos[i] == 0) {
            inodo_destino = i;
            break;
        }
    }
    if (inodo_destino == -1) {
        printf("Error: No hay inodos libres para crear el archivo destino.\n");
        return -1;
    }
    ext_bytemaps->bmap_inodos[inodo_destino] = 1;
    ext_superblock->s_free_inodes_count--;
    inodos->blq_inodos[inodo_destino] = inodos->blq_inodos[inodo_origen];
    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo == NULL_INODO) {
            strncpy(directorio[i].dir_nfich, nombredestino, LEN_NFICH);
            directorio[i].dir_nfich[LEN_NFICH-1] = '\0';
            directorio[i].dir_inodo = inodo_destino;
            break;
        }
    }
    printf("Archivo %s copiado a %s con éxito.\n", nombreorigen, nombredestino);
    return 0;
}

int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo) {
    int inodo_antiguo = BuscaFich(directorio, inodos, nombreantiguo);
    if (inodo_antiguo == NULL_INODO) {
        printf("Error: El archivo %s no existe.\n", nombreantiguo);
        return -1;
    }

    if (BuscaFich(directorio, inodos, nombrenuevo) != NULL_INODO) {
        printf("Error: El archivo %s ya existe.\n", nombrenuevo);
        return -1;
    }

    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (strcmp(directorio[i].dir_nfich, nombreantiguo) == 0) {
            strncpy(directorio[i].dir_nfich, nombrenuevo, LEN_NFICH);
            directorio[i].dir_nfich[LEN_NFICH - 1] = '\0';
            printf("Archivo %s renombrado a %s con éxito.\n", nombreantiguo, nombrenuevo);
            return 0;
        }
    }

    printf("Error: No se pudo renombrar el archivo %s.\n", nombreantiguo);
    return -1;
}
