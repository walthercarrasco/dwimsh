#ifndef BUILTINS_H
#define BUILTINS_H

#include "shell.h"

/**
 * @brief Estructura para definir comandos built-in
 */
typedef struct {
    /** Nombre del comando que el usuario debe escribir */
    const char *name;
    /** Puntero a la función que implementa el comando */
    void (*func)(char **args);
} BuiltInCommand;

// Declaraciones de funciones para comandos built-in
void cmd_cd(char **args);
void cmd_pwd(char **args);
void cmd_echo(char **args);
void cmd_exit(char **args);

// Array con los comandos built-in y su contador
extern BuiltInCommand builtin_commands[];
extern const int num_builtin_commands;

// Función para ejecutar comandos built-in
int execute_builtin(char **args);

#endif // BUILTINS_H 