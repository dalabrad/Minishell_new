# Guía de Parseo para Minishell 42

## 1. Definición de la Función main y Uso de envp

La función `main` es el punto de entrada de cualquier programa en C. Existen diversas formas de definirla:

- **`int main(void)`**  
  Se utiliza cuando no se requieren argumentos externos. El programa se inicia sin argumentos de línea de comandos.

- **`int main(int argc, char **argv)`**  
  Permite recibir argumentos de línea de comandos.  
  - `argc`: Número de argumentos pasados, incluyendo el nombre del programa.  
  - `argv`: Arreglo de cadenas de caracteres que contiene cada uno de los argumentos.

- **`int main(int argc, char **argv, char **envp)`**  
  Es una extensión común en sistemas Unix y Linux. Además de `argc` y `argv`, se incluye:
  - **`envp`**: Un arreglo (vector) de punteros a caracteres en el que cada posición apunta a una cadena con una variable de entorno. Cada cadena tiene el formato `NOMBRE=valor` y contiene información como:
    - **PATH:** Rutas de búsqueda de ejecutables.
    - **HOME:** Directorio personal del usuario.
    - **USER:** Nombre del usuario actual.
    
  Las variables de entorno son específicas de cada sesión de usuario, es decir, cada sesión puede tener un conjunto único de configuraciones. El arreglo `envp` termina con un puntero nulo (`NULL`), lo que indica el final de la lista.

Aunque el uso de `envp` no está definido por el estándar C, es muy común en entornos Unix/Linux ya que permite adaptar el comportamiento del programa según la configuración y el entorno de la sesión del usuario.

---

## 2. Tokens a Identificar

Los siguientes son los tokens principales que deben ser reconocidos y manejados durante el parseo:

- **Operadores especiales:**
  - `;` `|`
  - `<`, `<<`, `>`, `>>`
- **Comillas:**
  - `""` `''`
- **Variables de entorno:**
  - `$x`
- **Asignaciones:**
  - `x=y`
- **Posiciones especiales:**
  - Principio y final de la línea
- **Casos límite:**
  - `""''""` → `--------------------` `'-------------'`
  - Ejemplos combinados: `"'";'"'`

---

## 3. Primer Paso: Dividir el Prompt

Para interpretar correctamente el input, el primer paso es dividir la línea de comandos en tokens.

### Ejemplos de Segmentación

- **Entrada:** `cat archivo.txt;`  
  **Salida:** `[cat][archivo.txt]`

- **Entrada:** `cat archivo 2;`  
  **Salida:** `[cat][archivo][2]`

- **Entrada:** `cat "archivo 2"`  
  **Salida:** `[cat][archivo 2]`

### Regla para el Split

El prompt debe dividirse en tokens **por espacios explícitos que no están dentro de comillas dobles o simples**.

Ejemplo:
```sh
comando arg1 arg2 -> [comando][arg1][arg2]
````

---

## 4. Identificación de Elementos

La línea de comandos puede contener distintos tipos de elementos, por ejemplo:

```
> ENTRADA <SALIDA     X=Y $VAR      COMANDO ARG1 ARG2
```

### 4.1 Expansión de Variables

1. Identificar variables con `$VAR`.
2. Si existe un error en la sintaxis (por ejemplo, `[$][VAR]ERROR`), debe manejarse apropiadamente.
3. Una vez validadas, se sustituye el string `[$VAR]` por el valor correspondiente.
4. Se recomienda realizar la expansión **como último paso**.

Después de la expansión, la línea podría quedar:
```
> ENTRADA <SALIDA     X=Y CONTENIDO      COMANDO ARG1 ARG2
```

### 4.2 Redirecciones

Ejemplos de estructuras válidas e inválidas:

- Válido:
  - `[>][ENTRADA][<SALIDA][COMANDO][ARG1][ARG2]`
  - `[>ENTRADA]`
  - `[>][ENTRADA]`
  - `[<SALIDA]`
  - `[<][SALIDA]`
- Inválido (Errores de sintaxis):
  - `[>][]` → ERROR
  - `[<][]` → ERROR

Acciones:
- Guardar correctamente la entrada y la salida de la redirección.

### 4.3 Asignaciones

Casos correctos e incorrectos:

- Correcto:
  - `[X=Y]`
- Incorrecto:
  - `[X][=][Y]` → ERROR
  - `[X=][Y]` → ERROR???
  - `[X][=Y]` → ERROR

---

## 5. Identificar el Comando y sus Argumentos

Cuando no quedan más tokens especiales, se asume que la estructura restante sigue este formato:

```
"COMANDO ARG1 ARG2"
```

El primer elemento es el comando y el resto son los argumentos:
```
[COMANDO][ARG1][ARG2]
```

Ejemplos:

- Entrada: `ls -l /home`  
  Salida: `[ls][-l][/home]`

- Entrada: `echo "hola mundo"`  
  Salida: `[echo][hola mundo]`

---

## 6. Orden de Procesamiento Recomendado

1. **Tokenizar** la entrada respetando las comillas.
2. **Identificar redirecciones** (`<`, `>`, `|`, `;`) y separarlas.
3. **Identificar asignaciones** (`X=Y`).
4. **Expandir variables** (`$VAR`).
5. **Determinar el comando principal** y sus argumentos.
6. **Ejecutar** en el orden correcto, respetando las prioridades de cada elemento.