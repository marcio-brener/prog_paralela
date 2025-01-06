
#include "./ingredients.h"

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

int find_or_add(IngredientCount ***ingredients,  size_t *ingredients_size, size_t *allocated_size, const char *ingredient) {
    // Tentar encontrar o ingrediente existente
    for (int i = 0; i < *ingredients_size; i++) {
        if (strcmp((*ingredients)[i]->value, ingredient) == 0) {
            (*ingredients)[i]->count++;
            return 1;  // Ingrediente encontrado e atualizado
        }
    }

    // Verificar se precisamos expandir o array
    if (*ingredients_size == *allocated_size) {
        int new_size = (*allocated_size == 0) ? 1 : (*allocated_size * 2);  // Dobrar o tamanho alocado
        IngredientCount **temp = realloc(*ingredients, new_size * sizeof(IngredientCount *));
        if (temp == NULL) {
            printf("Erro ao alocar memória para os ingredientes!\n");
            return -1;  // Falha ao realocar memória
        }
        *ingredients = temp;
        *allocated_size = new_size;  // Atualiza o tamanho alocado
    }

    // Alocar e inicializar o novo ingrediente
    (*ingredients)[*ingredients_size] = malloc(sizeof(IngredientCount));
    if ((*ingredients)[*ingredients_size] == NULL) {
        printf("Erro ao alocar memória para o ingrediente!\n");
        return -1;  // Falha ao alocar memória
    }

    (*ingredients)[*ingredients_size]->value = strdup(ingredient);
    if ((*ingredients)[*ingredients_size]->value == NULL) {
        printf("Erro ao duplicar string!\n");
        free((*ingredients)[*ingredients_size]);  // Libera o espaço alocado para o ingrediente
        return -1;  // Falha ao duplicar string
    }

    // Inicializa o contador
    (*ingredients)[*ingredients_size]->count = 1;

    // Incrementa o tamanho da lista de ingredientes
    (*ingredients_size)++;

    return 0;  // Ingrediente adicionado com sucesso
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

void extract_ingredients(char *line, IngredientCount ***ingredients, size_t *ingredients_size, size_t *allocated_size) {
    if (line == NULL || ingredients == NULL || ingredients_size == NULL || allocated_size == NULL) {
        printf("Erro: Parâmetros inválidos fornecidos para extract_ingredients!\n");
        return;
    }

    char *start = strrchr(line, '[');
    char *end = strrchr(line, ']');

    if (start != NULL && end != NULL && start < end) {
        *(end + 1) = '\0';  // Termina a string após o ]
        char *ingredient_list = strdup(start + 1);  // Cria uma cópia segura da lista de ingredientes
        if (ingredient_list == NULL) {
            printf("Erro ao alocar memória para a lista de ingredientes!\n");
            return;
        }

        char *ingredient = strtok(ingredient_list, ",");
        while (ingredient != NULL) {
            // Remove espaços à esquerda
            while (*ingredient == ' ') ingredient++; 

            // Remove aspas e colchetes, se existirem
            remove_quotes_and_brackets(ingredient);

            // Verifica ou adiciona o ingrediente
            if (find_or_add(ingredients, ingredients_size, allocated_size, ingredient) == -1) {
                printf("Erro ao adicionar ingrediente: %s\n", ingredient);
                free(ingredient_list);  // Libera memória antes de retornar
                return;
            }

            ingredient = strtok(NULL, ",");
        }

        free(ingredient_list);  // Libera memória após uso
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