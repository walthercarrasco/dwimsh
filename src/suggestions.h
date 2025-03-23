#ifndef SUGGESTIONS_H
#define SUGGESTIONS_H

#include "shell.h"

// Funciones para sugerencia de comandos
int is_anagram(const char *s1, const char *s2);
float mi_fmin(float a, float b);
int levenshtein(const char *s1, const char *s2);
void suggest_command(const char *command, char *args[]);
void sigint_handler_suggest(int sig);

#endif // SUGGESTIONS_H 