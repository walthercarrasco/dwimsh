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
extern char **commands;
extern int command_count;
extern pid_t current_child_pid;
extern int foreground_process_running;
extern int last_command_status;
extern volatile sig_atomic_t suggestion_interrupted;

// Funciones principales
char* get_colored_prompt();
void handle_sigint(int sig);
void bin_commands();
char command_exists(const char *command);
void run_command(const char *command, char *args[], int background);

#endif // SHELL_H 