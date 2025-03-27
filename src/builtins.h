/**
 * @file builtins.h
 * @brief Definición de los comandos built-in de DWIMSH
 * 
 * Este archivo define la estructura y prototipos para los comandos
 * built-in de la shell, que son comandos implementados directamente
 * en el código de la shell sin necesidad de ejecutar programas externos.
 */

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
/**
 * @brief Implementa el comando cd para cambiar de directorio
 * @param args Argumentos del comando (args[1] es el directorio destino)
 */
void cmd_cd(char **args);

/**
 * @brief Implementa el comando pwd para mostrar el directorio actual
 * @param args Argumentos del comando (no utilizados)
 */
void cmd_pwd(char **args);

/**
 * @brief Implementa el comando echo para mostrar texto
 * @param args Argumentos del comando (texto a mostrar)
 */
void cmd_echo(char **args);

/**
 * @brief Implementa el comando exit para salir de la shell
 * @param args Argumentos del comando (no utilizados)
 */
void cmd_exit(char **args);

// Array con los comandos built-in y su contador
extern BuiltInCommand builtin_commands[];
extern const int num_builtin_commands;

/**
 * @brief Ejecuta un comando built-in si existe
 * @param args Argumentos del comando (args[0] es el nombre del comando)
 * @return 1 si se ejecutó un comando built-in, 0 si no
 */
int execute_builtin(char **args);

#endif // BUILTINS_H 