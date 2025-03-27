/**
 * @file suggestions.h
 * @brief Definición del sistema de sugerencias de comandos
 * 
 * Este archivo define las funciones necesarias para el sistema de sugerencias
 * de comandos, que se activa cuando el usuario introduce un comando que no existe.
 */

#ifndef SUGGESTIONS_H
#define SUGGESTIONS_H

#include "shell.h"

// Funciones para sugerencia de comandos
/**
 * @brief Determina si dos cadenas son anagramas entre sí
 * @param s1 Primera cadena
 * @param s2 Segunda cadena
 * @return 1 si son anagramas, 0 si no
 */
int is_anagram(const char *s1, const char *s2);

/**
 * @brief Devuelve el valor mínimo entre dos números de punto flotante
 * @param a Primer número
 * @param b Segundo número
 * @return El menor de los dos números
 */
float mi_fmin(float a, float b);

/**
 * @brief Calcula la distancia de Levenshtein entre dos cadenas
 * @param s1 Primera cadena
 * @param s2 Segunda cadena
 * @return Distancia de Levenshtein (número de ediciones necesarias)
 */
int levenshtein(const char *s1, const char *s2);

/**
 * @brief Sugiere comandos similares cuando se introduce uno que no existe
 * @param command Comando introducido por el usuario
 * @param args Argumentos del comando (se modificará args[0] si se acepta una sugerencia)
 */
void suggest_command(const char *command, char *args[]);

/**
 * @brief Maneja la señal SIGINT durante el proceso de sugerencias
 * @param sig Número de señal recibida
 */
void sigint_handler_suggest(int sig);

#endif // SUGGESTIONS_H 