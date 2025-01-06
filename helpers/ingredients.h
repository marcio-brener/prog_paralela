#ifndef INGREDIENTS_H
#define INGREDIENTS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>

// Estrutura para armazenar ingredientes e suas contagens
typedef struct {
    char *value;  // Nome do ingrediente
    int count;    // Número de ocorrências
} IngredientCount;

// Declarações de funções
int find_or_add(IngredientCount ***ingredients, size_t *ingredients_size, size_t *allocated_size, const char *ingredient);
void remove_quotes_and_brackets(char *str);
void extract_ingredients(char *line, IngredientCount ***ingredients, size_t *ingredients_size, size_t *allocated_size);
void sort_ingredients(IngredientCount **ingredients, int size);
void print_biggest_three_ingredients(IngredientCount **ingredients);
int count_ingredient_in_lines(FILE *file, const char *ingredient, int start_line, int end_line);


#endif
