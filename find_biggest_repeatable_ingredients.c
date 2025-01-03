#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Estrutura para armazenar os ingredientes e suas contagens
typedef struct {
    char *value;
    int count;
} IngredientCount;

// Função para comparar strings (usada para ordenar)
int compare(const void *a, const void *b) {
    return strcmp(((IngredientCount *)a)->value, ((IngredientCount *)b)->value);
}

// Função para procurar se o ingrediente já existe na lista de contagens
int find_or_add(IngredientCount ***ingredients, int *ingredients_size, const char *ingredient) {
    for (int i = 0; i < *ingredients_size; i++) {
        if (strcmp((*ingredients)[i]->value, ingredient) == 0) {
            // Se o ingrediente já existe, aumenta a contagem
            (*ingredients)[i]->count++;
            return 1;
        }
    }

    // Se o ingrediente não existe, adiciona um novo
    *ingredients_size += 1;
    *ingredients = realloc(*ingredients, (*ingredients_size) * sizeof(IngredientCount *));
    if (*ingredients == NULL) {
        printf("Erro ao alocar memória para os ingredientes!\n");
        return -1; // Retorna erro
    }
    
    (*ingredients)[*ingredients_size - 1] = malloc(sizeof(IngredientCount));
    if ((*ingredients)[*ingredients_size - 1] == NULL) {
        printf("Erro ao alocar memória para o ingrediente!\n");
        return -1; // Retorna erro
    }
    
    (*ingredients)[*ingredients_size - 1]->value = strdup(ingredient);
    (*ingredients)[*ingredients_size - 1]->count = 1;

    return 0;
}

// Função para remover aspas e colchetes de uma string
void remove_quotes_and_brackets(char *str) {
    char *src = str;
    char *dst = str;

    while (*src) {
        if (*src != '"' && *src != '[' && *src != ']') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';  // Terminador nulo
}

// Função para extrair os ingredientes da última ocorrência de colchetes
void extract_ingredients(char *line, IngredientCount ***ingredients, int *ingredients_size) {
    // Encontrar a última ocorrência dos colchetes
    char *start = strrchr(line, '[');  // Encontra o último '['
    char *end = strrchr(line, ']');    // Encontra o último ']'

    if (start != NULL && end != NULL && start < end) {
        *(end + 1) = '\0';  // Terminador nulo após o ']'
        char *ingredient_list = start + 1;

        // Separando os ingredientes entre as vírgulas
        char *ingredient = strtok(ingredient_list, ",");
        while (ingredient != NULL) {
            // Remove espaços extras
            while (*ingredient == ' ') ingredient++;

            // Remove aspas e colchetes
            remove_quotes_and_brackets(ingredient);

            // Adiciona o ingrediente ao grupo
            if (find_or_add(ingredients, ingredients_size, ingredient) == -1) {
                return; // Retorna em caso de erro de alocação
            }
            ingredient = strtok(NULL, ",");  // Pega o próximo ingrediente
        }
    }
}

// Função para ordenar os ingredientes pelo campo 'count' em ordem decrescente
void sort_ingredients(IngredientCount **ingredients, int size) {
    for (int i = 0; i < size - 1; i++) {
        int maxIndex = i;
        for (int j = i + 1; j < size; j++) {
            if (ingredients[j]->count > ingredients[maxIndex]->count) {
                maxIndex = j; // Encontra o maior elemento
            }
        }

        // Troca os ponteiros dos ingredientes
        if (maxIndex != i) {
            IngredientCount *temp = ingredients[i];
            ingredients[i] = ingredients[maxIndex];
            ingredients[maxIndex] = temp;
        }
    }
}

// Função para exibir os ingredientes
void print_biggest_three_ingredients(IngredientCount **ingredients) {
    for (int i = 0; i < 3; i++) {
        printf("%s: %d\n", ingredients[i]->value, ingredients[i]->count);
    }
}

int main() {
    FILE *file = fopen("test_dataset02.csv", "r");  // Abre o arquivo para leitura
    if (file == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }

    char line[1024];  // Buffer para armazenar cada linha
    IngredientCount **ingredients = malloc(0);  // Ponteiro para armazenar os ingredientes
    int ingredients_size = 0;  // Tamanho atual da lista de ingredientes

    // Lê o arquivo linha por linha
    while (fgets(line, sizeof(line), file)) {
        extract_ingredients(line, &ingredients, &ingredients_size);  // Extrai e conta os ingredientes
    }

    fclose(file);  // Fecha o arquivo

    // Ordena os ingredientes
    sort_ingredients(ingredients, ingredients_size);

    printf("\nIngredientes após a ordenação:\n");
    print_biggest_three_ingredients(ingredients);

    // Libera a memória
    for (int i = 0; i < ingredients_size; i++) {
        free(ingredients[i]->value);  // Libera a memória alocada para o nome do ingrediente
        free(ingredients[i]);         // Libera a estrutura de cada ingrediente
    }
    free(ingredients);  // Libera o array de ponteiros para ingredientes

    return 0;
}
