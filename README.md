# simul_ext

Este proyecto implementa un simulador simplificado de un sistema de archivos tipo Linux ext. El sistema se gestiona a través de un archivo binario `particion.bin` que simula una partición de disco.

## Descripción

El simulador permite realizar operaciones básicas sobre el sistema de archivos, tales como listar archivos, renombrar, copiar, eliminar e imprimir su contenido. Está diseñado para ser utilizado como parte de un proyecto educativo en la asignatura de Sistemas Operativos.

## Estructura del Proyecto

- **simul_ext.c**: Archivo principal del programa que contiene la lógica para procesar comandos.
- **cabeceras.h**: Archivo de cabecera que define las estructuras de datos utilizadas en el sistema.
- **particion.bin**: Archivo binario que simula la partición del disco.
- **README.md**: Este archivo.

## Checklist

- bytemaps ✅
- copy ✅
- dir ✅
- info ✅
- imprimir ✅
- rename ✅
- remove ✅
- salir ✅

<u>Extras</u>
  
- Inicio limpio ✅
- función clear ✅

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
   ```
##  Hecho por
   
 - Javier Dominguez
 - Alejandro Sastre


## Link al vídeo:
  [Vídeo](https://youtu.be/RTbDDGcOyDo)

<br>
<br>

| Elemento | Realizado | Comentario |
|----------|:---------:|------------|
| Arranque | ✅ | El encuadre inicial de las cabeceras y esqueleto main sin errores fue complicado. |
| Salir y mensajes de error | ✅ | N/A |
| info | ✅ | N/A |
| bytemaps | ✅ | N/A |
| dir | ✅ | N/A |
| rename | ✅ |  Al principio no éramos capaces de cambiar el nombre y el archivo “desaparecía”. Ese error fue solucionado y ahora funciona correctamente. |
| Remove | ✅ |  Una vez hecho el print de multibloque borrar fue algo parecido. BORRA MULTIBLOQUE |
| Imprimir | ✅ |  Ser capaces de leer archivos con más de un bloque fue complicado ya que no siempre accedíamos a los bloques correctos. LEE MULTIBLOQUE |
| Copiar | ✅ |  La lógica de copiar es parecida a la de print con la complicación de tener que buscar espacio libre para el nuevo archivo. COPIA MULTIBLOQUE |
| Commits, documentación y video | ✅ |  En cuanto a github no tuvimos muchas dificultades ya que estábamos familiarizados con la herramienta. |

