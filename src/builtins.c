/**
 * @file builtins.c
 * @brief Implementación de los comandos built-in de DWIMSH
 * 
 * Este archivo contiene la implementación de los comandos built-in
 * de la shell, como cd, pwd, echo y exit.
 */

#include "builtins.h"

/**
 * @brief Array con los comandos built-in disponibles
 */
BuiltInCommand builtin_commands[] = {
    {"cd", cmd_cd},
    {"pwd", cmd_pwd},
    {"echo", cmd_echo},
    {"exit", cmd_exit}
};

// Número de comandos built-in disponibles
const int num_builtin_commands = sizeof(builtin_commands) / sizeof(BuiltInCommand);

/**
 * @brief Implementa el comando built-in cd (cambio de directorio)
 * @param args Argumentos del comando (args[1] es el directorio destino)
 * 
 * Si no se proporciona un argumento, cambia al directorio HOME.
 */
void cmd_cd(char **args) {
    if (args[1] == NULL) {
        // Sin argumentos, cambiar al directorio HOME
        if (chdir(getenv("HOME")) != 0) {
            perror("cd: error al cambiar al directorio HOME");
            last_command_status = 1;
        } else {
            last_command_status = 0;
        }
    } else {
        // Cambiar al directorio especificado
        if (chdir(args[1]) != 0) {
            perror("cd: error al cambiar de directorio");
            last_command_status = 1;
        } else {
            last_command_status = 0;
        }
    }
}

/**
 * @brief Implementa el comando built-in pwd (muestra directorio actual)
 * @param args Argumentos del comando (no utilizados)
 */
void cmd_pwd(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        last_command_status = 0;
    } else {
        perror("pwd: error al obtener el directorio actual");
        last_command_status = 1;
    }
}

/**
 * @brief Implementa el comando built-in echo (muestra texto)
 * @param args Argumentos del comando (texto a mostrar)
 * 
 * Imprime todos los argumentos separados por espacios.
 */
void cmd_echo(char **args) {
    // Imprimir todos los argumentos separados por espacios
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s", args[i]);
        if (args[i+1] != NULL) {
            printf(" ");
        }
    }
    printf("\n");
    last_command_status = 0;
}

/**
 * @brief Implementa el comando built-in exit (salir de la shell)
 * @param args Argumentos del comando (no utilizados)
 */
void cmd_exit(char **args) {
    printf("Saliendo de dwimsh...\n");
    exit(0);
}

/**
 * @brief Intenta ejecutar un comando built-in
 * @param args Argumentos del comando (args[0] es el nombre del comando)
 * @return 1 si se ejecutó un comando built-in, 0 si no
 * 
 * Busca el comando en la lista de comandos built-in y lo ejecuta si existe.
 */
int execute_builtin(char **args) {
    if (args[0] == NULL) return 0;
    
    for (int i = 0; i < num_builtin_commands; i++) {
        if (strcmp(args[0], builtin_commands[i].name) == 0) {
            builtin_commands[i].func(args);
            return 1;
        }
    }
    
    return 0; // No es un comando built-in
} 