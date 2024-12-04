# simul_ext

Este proyecto implementa un simulador simplificado de un sistema de archivos tipo Linux ext. El sistema se gestiona a través de un archivo binario `particion.bin` que simula una partición de disco.

## Descripción

El simulador permite realizar operaciones básicas sobre el sistema de archivos, tales como listar archivos, renombrar, copiar, eliminar e imprimir su contenido. Está diseñado para ser utilizado como parte de un proyecto educativo en la asignatura de Sistemas Operativos.

## Estructura del Proyecto

- **simul_ext.c**: Archivo principal del programa que contiene la lógica para procesar comandos.
- **cabeceras.h**: Archivo de cabecera que define las estructuras de datos utilizadas en el sistema.
- **particion.bin**: Archivo binario que simula la partición del disco.
- **README.md**: Este archivo.

## Instalación

1. Clona el repositorio:

   ```bash
   git clone https://github.com/usuario/simul_ext.git
   
2. Compila el programa:

   ```bash
   gcc -o simul_ext simul_ext.c

## Uso

Ejecuta el programa desde la línea de comandos:

   ```bash
   ./simul_ex
