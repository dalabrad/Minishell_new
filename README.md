# Minishell

This project is a simplified Unix shell developed in C. It replicates the basic behavior of Bash: command input, parsing, execution, redirections, pipes, environment variables, built-ins, and signal handling. The goal is to understand how a shell works internally and manage memory, file descriptors, and system calls at a low level.

## Features

- Prompt and line reading
- Command parsing with support for:
  - Quotes (`'`, `"`)
  - Pipes (`|`)
  - Redirections (`>`, `>>`, `<`, `<<`)
  - Environment variable expansion (`$VAR`, `$?`)
- Execution of system binaries and built-in commands
- Built-ins implemented:
  - `echo`
  - `cd`
  - `pwd`
  - `export`
  - `unset`
  - `env`
  - `exit`
- Heredocs (`<<`) with temporary file management
- Signal handling (`SIGINT`, `SIGQUIT`, etc.)
- History support via readline

## Requirements

- GCC and Make
- GNU Readline library

## Compilation

```bash
make

## Running the Shell

./minishell

Use the prompt to enter commands like you would in Bash. To exit:

exit

## Examples

echo "Hello World"
ls -l | grep txt > result.txt
cat < result.txt
export NAME=Vanessa
echo $NAME

## File Structure

    src/ – Source files grouped by logic (parsing, execution, utils...)

    inc/ – Header files

    libft/ – Custom implementation of common C library functions

    Makefile – Compilation rules

    .minishell_history – Saved command history (created at runtime)

## Bonus

    Handling of quotes with and without expansion

    Management of multiple pipes and redirections

    Full support for environment variable expansion in command arguments

    Leak-free memory management validated with Valgrind