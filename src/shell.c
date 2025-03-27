/**
 * @file shell.c
 * @brief Implementación principal de la shell DWIMSH
 * 
 * Este archivo contiene la implementación de las funciones principales
 * de la shell, incluyendo el bucle principal, manejo de señales,
 * ejecución de comandos y gestión del prompt.
 * 
 * @author Walther Carrasco
 */

#include "shell.h"
#include "builtins.h"
#include "suggestions.h"

// Variables globales
char **commands = NULL;
int command_count = 0;
pid_t current_child_pid = 0;
int foreground_process_running = 0;
int last_command_status = 0;
volatile sig_atomic_t suggestion_interrupted = 0;

/**
 * @brief Genera el prompt con color según el estado del último comando
 * @return Cadena con el prompt formateado (verde si exitoso, rojo si falló)
 */
char* get_colored_prompt() {
    static char prompt[20];
    
    if (last_command_status == 0) {
        sprintf(prompt, "%sdwimsh>%s ", COLOR_GREEN, COLOR_RESET);
    } else {
        sprintf(prompt, "%sdwimsh>%s ", COLOR_RED, COLOR_RESET);
    }
    
    return prompt;
}

/**
 * @brief Maneja la señal SIGINT (Ctrl+C)
 * @param sig Número de señal recibida
 * 
 * Si hay un proceso en primer plano, envía la señal a ese proceso.
 * Si no, muestra un nuevo prompt limpio.
 */
void handle_sigint(int sig) {
    if (foreground_process_running && current_child_pid > 0) {
        kill(current_child_pid, SIGINT);
        last_command_status = 1;
        printf("\n");
    } else {
        last_command_status = 1;
        printf("\n");
        // Borrar la línea actual y mostrar un nuevo prompt limpio
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

/**
 * @brief Carga la lista de comandos disponibles del sistema
 * 
 * Busca en /usr/bin todos los archivos ejecutables y los carga en
 * la lista global de comandos disponibles. También añade los comandos
 * built-in a la lista.
 */
void bin_commands() {
    const char *path = "/usr/bin";

    command_count = 0; 
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL){
        perror("Error al abrir el directorio");
        return;
    }
    
    char sub_path[1024];
    // Contamos cuántos archivos hay en el directorio
    while ((entry = readdir(dir)) != NULL){
        snprintf(sub_path, sizeof(sub_path), "%s/%s", path, entry->d_name);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (access(sub_path, X_OK) == 0)
            command_count++;
    }
    closedir(dir);

    commands = (char **)malloc((command_count + 4) * sizeof(char *));

    if (commands == NULL) {
        perror("Error al asignar memoria");
        return;
    }

    dir = opendir(path);
    if (dir == NULL){
        perror("Error al abrir el directorio");
        for (int j = 0; j < command_count; j++){
            free(commands[j]);
        }
        free(commands);
        closedir(dir);
        return;
    }

    int i = 0;
    while ((entry = readdir(dir)) != NULL && i < command_count){
        snprintf(sub_path, sizeof(sub_path), "%s/%s", path, entry->d_name);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (access(sub_path, X_OK) == 0){
            commands[i] = strdup(entry->d_name);
            i++;
        }
    }
    
    commands[i] = strdup("pwd");
    commands[i+1] = strdup("echo");
    commands[i+2] = strdup("exit");
    commands[i+3] = strdup("cd");
    command_count += 4; // reserva para pwd, echo, exit, cd
    closedir(dir);
}

/**
 * @brief Verifica si un comando existe en el PATH del sistema
 * @param command Nombre del comando a verificar
 * @return 1 si existe, 0 si no existe
 */
char command_exists(const char *command) {
    char *path = getenv("PATH");
    char *path_copy = strdup(path);
    if (!path)
        return 0;
        
    char sub_path[1024];
    char *dir = strtok(path_copy, ":");
    while (dir){
        snprintf(sub_path, sizeof(sub_path), "%s/%s", dir, command);
        if (access(sub_path, X_OK) == 0){
            free(path_copy);
            return 1;
        }    
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return 0;
}

/**
 * @brief Ejecuta un comando externo o built-in
 * @param command Nombre del comando a ejecutar
 * @param args Arreglo de argumentos (incluyendo el comando)
 * @param background Indica si el comando se ejecuta en segundo plano (1) o primer plano (0)
 * 
 * Primero verifica si es un comando built-in, si no, lo ejecuta como proceso externo.
 * Actualiza last_command_status con el estado de salida del comando.
 */
void run_command(const char *command, char *args[], int background) {
    // Primero verificar si es un comando built-in
    if (execute_builtin(args)) {
        return;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        last_command_status = 1; // Error
        exit(1);
    }
    else if (pid == 0) { /* Child process */
        // Restaura el comportamiento por defecto de SIGINT en el proceso hijo
        signal(SIGINT, SIG_DFL);
        execvp(args[0], args);
        perror("Execution failed"); /* If execvp fails */
        exit(1); // Salir con error
    }
    else { /* Parent process */
        if (!background) {
            // Actualiza las variables globales
            current_child_pid = pid;
            foreground_process_running = 1;
            
            int status;
            waitpid(pid, &status, 0); /* Wait for child if not background */
            
            // Verificar el estado de salida del proceso hijo
            if (WIFEXITED(status)) {
                last_command_status = WEXITSTATUS(status); // Usar el código de salida real
            } else if (WIFSIGNALED(status)) {
                last_command_status = 1; // Terminado por señal = error
            }
            
            // Restablece las variables globales
            current_child_pid = 0;
            foreground_process_running = 0;
        } else {
            // Para procesos en background, asumimos éxito a menos que sepamos lo contrario
            last_command_status = 0;
        }
    }
}

/**
 * @brief Función principal de la shell
 * @return Estado de salida de la shell
 * 
 * Implementa el bucle principal de la shell, leyendo comandos del usuario,
 * procesándolos y ejecutándolos.
 */
int main(void) {
    char *inputBuffer;
    int background;
    char *args[MAX_LINE / +1];

    // Configura el manejador de señal para SIGINT
    signal(SIGINT, handle_sigint);

    rl_bind_key('\t', rl_complete);

    printf("Bienvenido a dwimsh - Escrito por Walther Carrasco\n");
    bin_commands();
    
    while (1) { 
        background = 0;
        
        // Usa readline para obtener input con prompt coloreado
        inputBuffer = readline(get_colored_prompt());
        
        // Si el usuario presiona Ctrl+D, inputBuffer será NULL
        if (inputBuffer == NULL) {
            printf("\nSaliendo de dwimsh...\n");
            break;
        }
        
        // Si el comando no está vacío, añadirlo al historial
        if (*inputBuffer) {
            add_history(inputBuffer);
        }
        
        // Parsear la línea para obtener los argumentos
        char *token;
        int i = 0;
        token = strtok(inputBuffer, " \t\n");
        
        while (token != NULL && i < MAX_LINE/2) {
            args[i] = token;
            token = strtok(NULL, " \t\n");
            i++;
        }
        args[i] = NULL;
        
        // Verificar si es un comando en background
        if (i > 0 && strcmp(args[i-1], "&") == 0) {
            background = 1;
            args[i-1] = NULL;
        }
        
        if (args[0] == NULL) {
            free(inputBuffer);
            continue;
        }

        if (execute_builtin(args)){
            free(inputBuffer);
            continue;
        }

        if (!command_exists(args[0])){
            suggest_command(args[0], args);
            if (args[0] == NULL){
                printf("No entiendo que quiere hacer, pruebe de nuevo.\n");
                last_command_status = 1; // Marcar como error
                free(inputBuffer);
                continue;
            }
            run_command(args[0], args, background);
        } else {
            run_command(args[0], args, background);
        }
        
        free(inputBuffer);  // Importante liberar la memoria asignada por readline
    }
    
    // Limpieza de readline al salir
    clear_history();
    
    printf("Saliendo de dwimsh...\n");
    return 0;
}
