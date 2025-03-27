/**
 * @file suggestions.c
 * @brief Implementación del sistema de sugerencias de comandos
 * 
 * Este archivo contiene la implementación de las funciones que permiten
 * sugerir comandos similares cuando el usuario introduce un comando
 * que no existe. Utiliza algoritmos como la distancia de Levenshtein
 * y detección de anagramas.
 */

#include "suggestions.h"

/**
 * @brief Devuelve el valor mínimo entre dos números de punto flotante
 * @param a Primer número
 * @param b Segundo número
 * @return El menor de los dos números
 */
float mi_fmin(float a, float b) {
    return (a < b) ? a : b;
}

/**
 * @brief Calcula la distancia de Levenshtein entre dos cadenas
 * @param s1 Primera cadena
 * @param s2 Segunda cadena
 * @return Distancia de Levenshtein (número de ediciones necesarias)
 * 
 * Implementa el algoritmo de distancia de Levenshtein para medir
 * la similitud entre dos cadenas.
 */
int levenshtein(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int matriz[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) {
        matriz[i][0] = i;
    }
    for (int j = 0; j <= len2; j++) {
        matriz[0][j] = j;
    }

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int costo = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            matriz[i][j] = mi_fmin(matriz[i - 1][j] + 1, mi_fmin(matriz[i][j - 1] + 1, matriz[i - 1][j - 1] + costo));
        }
    }

    return matriz[len1][len2];
}

/**
 * @brief Determina si dos cadenas son anagramas entre sí
 * @param s1 Primera cadena
 * @param s2 Segunda cadena
 * @return 1 si son anagramas, 0 si no
 * 
 * Dos cadenas son anagramas si contienen exactamente los mismos
 * caracteres pero en diferente orden.
 */
int is_anagram(const char *s1, const char *s2) {
    if (strlen(s1) != strlen(s2)) {
        return 0;
    }
    
    int count[256] = {0};
    
    for (int i = 0; s1[i]; i++) {
        count[(unsigned char)s1[i]]++;
    }
    
    for (int i = 0; s2[i]; i++) {
        count[(unsigned char)s2[i]]--;
        if (count[(unsigned char)s2[i]] < 0) {
            return 0;
        }
    }
    
    for (int i = 0; i < 256; i++) {
        if (count[i] != 0) {
            return 0;
        }
    }
    
    return 1;
}

/**
 * @brief Maneja la señal SIGINT durante el proceso de sugerencias
 * @param sig Número de señal recibida
 * 
 * Establece la bandera suggestion_interrupted para cancelar el
 * proceso de sugerencias.
 */
void sigint_handler_suggest(int sig) {
    suggestion_interrupted = 1;
    last_command_status = 1;
}

/**
 * @brief Sugiere comandos similares cuando se introduce uno que no existe
 * @param command Comando introducido por el usuario
 * @param args Argumentos del comando (se modificará args[0] si se acepta una sugerencia)
 * 
 * Busca comandos similares utilizando dos métodos:
 * 1. Busca anagramas exactos
 * 2. Busca comandos con distancia de Levenshtein pequeña
 * 
 * Luego pregunta al usuario si desea utilizar alguna de las sugerencias.
 */
void suggest_command(const char *command, char *args[]) {
    char *suggestions[20];
    int count = 0;
    
    struct sigaction sa_old, sa_new;
    sigaction(SIGINT, NULL, &sa_old);
    
    sa_new.sa_handler = sigint_handler_suggest;
    sigemptyset(&sa_new.sa_mask);
    sa_new.sa_flags = 0;
    sigaction(SIGINT, &sa_new, NULL);
    
    suggestion_interrupted = 0;
    
    // Buscar sugerencias - primero anagramas
    for (int i = 0; i < command_count && count < 10; i++) {
        if (is_anagram(command, commands[i])) {
            suggestions[count] = strdup(commands[i]);
            count++;
        }
    }
    
    int max_distance = strlen(command) > 3 ? 2 : 1;
    // Luego buscamos por distancia de Levenshtein, evitando duplicados
    for (int i = 0; i < command_count && count < 20; i++) {
        int already_added = 0;
        for (int j = 0; j < count; j++) {
            if (strcmp(commands[i], suggestions[j]) == 0) {
                already_added = 1;
                break;
            }
        }
        
        if (!already_added) {
            int distance = levenshtein(command, commands[i]);
            if (distance <= max_distance) {
                suggestions[count] = strdup(commands[i]);
                count++;
            }
        }
    }
    
    if (count == 0 || suggestion_interrupted) {
        sigaction(SIGINT, &sa_old, NULL);
        args[0] = NULL;
        return;
    }
    
    char full_command_with_args[MAX_LINE * 2];
    int suggestion_index = 0;
    char response[16]; 

    while (suggestion_index < count && !suggestion_interrupted) {
        strcpy(full_command_with_args, suggestions[suggestion_index]);
        for (int i = 1; args[i] != NULL; i++) {
            strcat(full_command_with_args, " ");
            strcat(full_command_with_args, args[i]);
        }
        
        printf("¿Quieres decir \"%s\"? [s/n] ", full_command_with_args);
        fflush(stdout);
        
        if (fgets(response, sizeof(response), stdin) == NULL || suggestion_interrupted) {
            break;
        }

        response[strcspn(response, "\n")] = '\0';

        if (strcmp(response, "s") == 0) {
            args[0] = strdup(suggestions[suggestion_index]);
            break;
        } else if (strcmp(response, "n") == 0) {
            suggestion_index++;
        }
    }
    
    if (suggestion_index >= count || suggestion_interrupted) {
        args[0] = NULL;
        last_command_status = 1;
    } else {
        last_command_status = 0;
    }
    
    for (int i = 0; i < count; i++) {
        free(suggestions[i]);
    }
    
    sigaction(SIGINT, &sa_old, NULL);
} 