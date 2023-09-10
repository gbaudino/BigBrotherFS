# Informe laboratorio 4 - Grupo 21

## Integrantes

- Antequera Ezequiel  
  eantequera404@mi.unc.edu.ar
- Baudino Geremias  
  geremiasbaudino@mi.unc.edu.ar
- Guglieri Juan Cruz  
  juan.cruz.guglieri@mi.unc.edu.ar
- Mollea Maria Agustina  
  agustina.mollea@mi.unc.edu.ar  


### Indice
 1. [Q&A](#qa)
 2. [Puntos Estrella](#puntos-estrella)
 3. [Sobre Nuestro Codigo](#sobre-nuestro-codigo)

## Q&A
1. Cuando se ejecuta el main con la opción -d, ¿qué se está mostrando en la pantalla?  
Con la opción -d se ejecuta el modo debug de FUSE, el cual nos permite ver en pantalla un log en tiempo real de todas las llamadas a funciones y operaciones realizadas, así como también los mensajes de debug a los que la ejecución del programa accede.
<br>
2. ¿Hay alguna manera de saber el nombre del archivo guardado en el cluster 157?  
Para saber el nombre de un archivo guardado en determinado cluster, lo que deberia de hacerse en primer lugar es ver si dicho cluster está libre y en ese caso se puede asegurar que ningun archivo se encuentra allí. Caso contrario hay que buscar en el "tree" si algún archivo tiene como start cluster el cluster 157, para ello existe una función que devuelve NULL en caso de no encontrarlo y sino devuelve el archivo, donde se puede acceder a su nombre, pero en caso de no encontrarlo y recibir NULL, se debería buscar si algun cluster referencia a este, de encontrar alguno debemos repetir el proceso de fijarse si existe algun archivo con ese cluster como start cluster, sino, repetir este proceso hasta descartar toda posibilidad.
<br>
3. ¿Dónde se guardan las entradas de directorio? ¿Cuántos archivos puede tener adentro un directorio en FAT32?  
Las entradas de directorio se guardan en el cluster correspondiente al directorio. En FAT32 el tamaño máximo de un directorio es de 65536 entradas de 32 bits, suponiendo que todas ellas son archivos, entonces tenemos 65536 archivos como maximo en un directorio, pero particularmente en nuestra implementación (otorgado así por la cátedra) solo podemos tener 16 entradas por directorio como máximo. Esto se debe a que cada cluster tiene 512 bytes(determinado por la cátedra) y cada direntry ocupa 32 bytes, lo que nos da como resultado 16 entradas por cluster, al querer agregar una nueva entrada al directorio deberiía de añadirse otro cluster y eso no es manejado en la implementación actual.  
<br>
4. Cuando se ejecuta el comando como ls -l, el sistema operativo, ¿llama a algún programa de usuario? ¿A alguna llamada al sistema? ¿Cómo se conecta esto con FUSE? ¿Qué funciones de su código se ejecutan finalmente?  
Cuando se ejecuta el comando ls -l, el modulo FUSE en primera instancia llama a la función `OPENDIR`, la cual se encarga de abrir el directorio "root", una vez abierto van a obtenerse los "atributos" a través de la función `GETATTR`, y por último, al ser un directorio, va a llamarse a la función `READDIR`, la cual se encarga de leer y cargar en el "tree" a todos los hijos del root. Esto, se repite sucesivamente para cada archivo y directorio que se encuentre en el "tree", con la diferencia que en los directorios se repetirá lo de leer sus hijos con `READDIR` y en los archivos no.
<br>
5. ¿Por qué tienen que escribir las entradas de directorio manualmente pero no tienen que guardar la tabla FAT cada vez que la modifican?  
Una vez asignado el cluster, la FAT table ya no se modifica, sino que lo que cambia al modificar el archivo es el contenido del cluster.
<br>
6. Para los sistemas de archivos FAT32, la tabla FAT, ¿siempre tiene el mismo tamaño? En caso de que sí, ¿qué tamaño tiene?
La tabla FAT siempre tiene el mismo tamaño una vez que se crea el sistema de archivos, y su tamaño es de 512 bytes por cada cluster (en este caso particular). Por lo que la tabla FAT depende del tamaño de la imagen de la que se hable. También es importante mencionar que la tabla FAT cuenta con un limite, esta solo puede "apuntar" a 8TB de información y como máximo cada archivo puede tener 4GB de información.


## Puntos Estrella
Acerca de los puntos estrella, se han implementado los siguientes:
- **Punto 1**: Por implementar.
- **Punto 2**: Por implementar.
- **Punto 3**: Implementado. Se pueden ver las funciones añadidas al código en el archivo `src/fuse_operations.c` y `src/big_brother.c`.
- **Punto 7**: Implementado pero no estable. Codigo comentado en la función `write_dir_entry` del archivo `src/fat_file.c`. Funciona correctamente hasta que se desmonta y vuelve a montar la imagen(se rompe).


## Sobre Nuestro Codigo
- Todas las funciones core, e incluso algunas extra, fueron implementadas y probadas con éxito.  
- El código pasa todos los tests de la cátedra. Probar en el directorio `src` con `make test`.
- Se implementó un script facilitador del compilado, ejecución, montado y desmontado de la imagen. Para usarlo, ejecutar **`./exec.sh`** en el directorio `src`, pudiendo pasarle como argumento el nombre de la imagen a montar (por defecto `fatfs.img`), si queremos hacer una imagen nueva (agregar "new" como segundo parámetro) o usar la misma que en un montaje anterior (por defecto), o si quermeos hacer un montaje con parámetros (agregar el parámetro como segundo, si no se uso new, o tercer parámetro). Algunos ejemplos: `./exec.sh`, `./exec.sh bb_fs new`, `./exec.sh file d`, `./exec.sh bb_fs new d`.