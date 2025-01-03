#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "./helpers/ingredients.h"

typedef struct {
    char *value;
    int count;
} IngredientCount;

int main() {
    FILE *file = fopen("test_dataset02.csv", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }

    char line[1024];
    IngredientCount **ingredients = malloc(0);
    int ingredients_size = 0;

    while (fgets(line, sizeof(line), file)) {
        extract_ingredients(line, &ingredients, &ingredients_size);
    }

    fclose(file);
    sort_ingredients(ingredients, ingredients_size);

    printf("\nIngredientes após a ordenação:\n");
    print_biggest_three_ingredients(ingredients);

    for (int i = 0; i < ingredients_size; i++) {
        free(ingredients[i]->value);
        free(ingredients[i]);
    }
    free(ingredients);

    return 0;
}
