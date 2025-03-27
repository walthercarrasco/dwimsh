/**
 * @file shell.h
 * @brief Cabecera principal para la implementación de DWIMSH (Do What I Mean SHell)
 * 
 * Este archivo define las estructuras, constantes y prototipos de funciones 
 * necesarios para la implementación de la shell interactiva DWIMSH.
 * Incluye declaraciones de variables globales y funciones principales
 * para el manejo de comandos, señales y prompt.
 */

#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

// Declaraciones de funciones de readline que usamos
extern void rl_replace_line(const char *text, int clear_undo);
extern int rl_on_new_line(void);
extern void rl_redisplay(void);

// Tamaño máximo de línea de comando
#define MAX_LINE 80

// Códigos ANSI para colorear la salida en terminal
#define COLOR_GREEN  "\033[32m"
#define COLOR_RED    "\033[31m"
#define COLOR_RESET  "\033[0m"

// Variables globales exportadas
extern char **commands;         /**< Lista de comandos disponibles */
extern int command_count;       /**< Número de comandos disponibles */
extern pid_t current_child_pid; /**< PID del proceso hijo actualmente en ejecución */
extern int foreground_process_running; /**< Indica si hay un proceso en primer plano */
extern int last_command_status; /**< Estado de salida del último comando ejecutado */
extern volatile sig_atomic_t suggestion_interrupted; /**< Indica si se interrumpió una sugerencia */

// Funciones principales
/**
 * @brief Genera el prompt coloreado según el estado del último comando
 * @return Cadena con el prompt formateado
 */
char* get_colored_prompt();

/**
 * @brief Maneja la señal SIGINT (Ctrl+C)
 * @param sig Número de señal recibida
 */
void handle_sigint(int sig);

/**
 * @brief Carga los comandos disponibles del sistema
 */
void bin_commands();

/**
 * @brief Verifica si un comando existe en el PATH
 * @param command Nombre del comando a verificar
 * @return 1 si existe, 0 si no existe
 */
char command_exists(const char *command);

/**
 * @brief Ejecuta un comando externo o built-in
 * @param command Nombre del comando a ejecutar
 * @param args Arreglo de argumentos (incluyendo el comando)
 * @param background Indica si el comando se ejecuta en segundo plano
 */
void run_command(const char *command, char *args[], int background);

#endif // SHELL_H 