#include "ingredients.h"

#define MAX_LINE_LENGTH 1024

int count_ingredient_in_lines(FILE *file, const char *ingredient, int start_line, int end_line) {
    char line[MAX_LINE_LENGTH];
    int count = 0;

    for (int i = 0; i < start_line; i++) {
        if (fgets(line, MAX_LINE_LENGTH, file) == NULL) {
            return 0;
        }
    }
    
    for (int i = start_line; i < end_line; i++) {
        if (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
            if (strstr(line, ingredient) != NULL) {
                count++;
            }
        }
    }

    return count;
}

int find_or_add(IngredientCount ***ingredients, int *ingredients_size, const char *ingredient) {
    for (int i = 0; i < *ingredients_size; i++) {
        if (strcmp((*ingredients)[i]->value, ingredient) == 0) {
            (*ingredients)[i]->count++;
            return 1;
        }
    }

    *ingredients_size += 1;
    *ingredients = realloc(*ingredients, (*ingredients_size) * sizeof(IngredientCount *));
    if (*ingredients == NULL) {
        printf("Erro ao alocar memória para os ingredientes!\n");
        return -1;
    }
    
    (*ingredients)[*ingredients_size - 1] = malloc(sizeof(IngredientCount));
    if ((*ingredients)[*ingredients_size - 1] == NULL) {
        printf("Erro ao alocar memória para o ingrediente!\n");
        return -1;
    }
    
    (*ingredients)[*ingredients_size - 1]->value = strdup(ingredient);
    (*ingredients)[*ingredients_size - 1]->count = 1;

    return 0;
}

void remove_quotes_and_brackets(char *str) {
    char *src = str;
    char *dst = str;

    while (*src) {
        if (*src != '"' && *src != '[' && *src != ']') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

void extract_ingredients(char *line, IngredientCount ***ingredients, int *ingredients_size) {
    char *start = strrchr(line, '[');
    char *end = strrchr(line, ']');

    if (start != NULL && end != NULL && start < end) {
        *(end + 1) = '\0';
        char *ingredient_list = start + 1;

        char *ingredient = strtok(ingredient_list, ",");
        while (ingredient != NULL) {
            while (*ingredient == ' ') ingredient++;

            remove_quotes_and_brackets(ingredient);

            if (find_or_add(ingredients, ingredients_size, ingredient) == -1) {
                return;
            }
            ingredient = strtok(NULL, ",");
        }
    }
}

void sort_ingredients(IngredientCount **ingredients, int size) {
    for (int i = 0; i < size - 1; i++) {
        int maxIndex = i;
        for (int j = i + 1; j < size; j++) {
            if (ingredients[j]->count > ingredients[maxIndex]->count) {
                maxIndex = j;
            }
        }

        if (maxIndex != i) {
            IngredientCount *temp = ingredients[i];
            ingredients[i] = ingredients[maxIndex];
            ingredients[maxIndex] = temp;
        }
    }
}

void print_biggest_three_ingredients(IngredientCount **ingredients) {
    for (int i = 0; i < 3; i++) {
        printf("%s: %d\n", ingredients[i]->value, ingredients[i]->count);
    }
}