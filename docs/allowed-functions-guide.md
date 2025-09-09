# Resumen de Funciones Permitidas

## Funciones de **readline**

1. **`readline`**: Lee una línea desde la entrada estándar con edición interactiva. Devuelve la línea como `char*`.
2. **`rl_clear_history`**: Limpia el historial de líneas guardadas.
3. **`rl_on_new_line`**: Indica que se ha comenzado una nueva línea.
4. **`rl_replace_line`**: Reemplaza el contenido actual por una nueva línea.
5. **`rl_redisplay`**: Refresca y muestra la línea actual en pantalla.
6. **`add_history`**: Añade una línea al historial.

## Funciones estándar

7. **`printf`**: Imprime texto formateado en la salida estándar.
8. **`malloc`**: Reserva un bloque de memoria dinámicamente.
9. **`free`**: Libera memoria previamente reservada.
10. **`write`**: Escribe datos a un descriptor de archivo.
11. **`access`**: Verifica si un archivo es accesible con permisos dados.
12. **`open`**: Abre un archivo y devuelve un descriptor.
13. **`read`**: Lee datos desde un descriptor.
14. **`close`**: Cierra un descriptor de archivo.
15. **`fork`**: Crea un proceso hijo duplicando el actual.
16. **`wait`**: Espera a que un hijo termine.
17. **`waitpid`**: Espera a un proceso hijo específico.
18. **`wait3` y `wait4`**: Esperan procesos hijos con opciones avanzadas.
19. **`signal`**: Configura un manejador para una señal.
20. **`sigaction`**: Configura manejo avanzado de señales.
21. **`kill`**: Envía una señal a un proceso.
22. **`exit`**: Termina el proceso con un estado dado.
23. **`getcwd`**: Obtiene el directorio actual.
24. **`chdir`**: Cambia el directorio actual.
25. **`stat`**: Obtiene información sobre un archivo.
26. **`lstat`**: Igual que `stat`, pero no sigue enlaces simbólicos.
27. **`fstat`**: Obtiene información sobre un descriptor.
28. **`unlink`**: Elimina un archivo.
29. **`execve`**: Ejecuta un nuevo programa reemplazando el proceso actual.
30. **`dup` y `dup2`**: Duplican descriptores de archivo.
31. **`pipe`**: Crea un canal unidireccional para procesos.

## Funciones de directorio

32. **`opendir`**: Abre un directorio.
33. **`readdir`**: Lee una entrada del directorio.
34. **`closedir`**: Cierra el directorio.

## Funciones de error

35. **`strerror`**: Devuelve una descripción del error asociado a `errno`.
36. **`perror`**: Imprime el mensaje de error en `stderr`.

## Funciones de terminal

37. **`isatty`**: Verifica si un descriptor es un terminal.
38. **`ttyname`**: Obtiene el nombre del terminal.
39. **`ttyslot`**: Devuelve el número del terminal.
40. **`ioctl`**: Controla dispositivos.
41. **`getenv`**: Obtiene el valor de una variable de entorno.
42. **`tcsetattr`**: Configura atributos del terminal.
43. **`tcgetattr`**: Obtiene atributos actuales del terminal.

## Funciones termcap

44. **`tgetent`**: Inicializa el acceso a termcap.
45. **`tgetflag`**: Obtiene un valor booleano.
46. **`tgetnum`**: Obtiene un valor numérico.
47. **`tgetstr`**: Obtiene una cadena termcap.
48. **`tgoto`**: Genera una cadena que mueve el cursor.
49. **`tputs`**: Envía una cadena termcap a la salida.

---

