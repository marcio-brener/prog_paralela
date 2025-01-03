#include "./helpers/ingredients.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    FILE *file = fopen("test_dataset02.csv", "r");
    if (file == NULL) {
        if (rank == 0) {
            printf("Erro ao abrir o arquivo!\n");
        }
        MPI_Finalize();
        return 1;
    }

    char line[1024];
    IngredientCount **ingredients = malloc(0);
    int ingredients_size = 0;

    // Distribute lines to each process
    int line_count = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line_count % size == rank) { // Only process lines assigned to this rank
            extract_ingredients(line, &ingredients, &ingredients_size);
        }
        line_count++;
    }

    fclose(file);

    // Gather all ingredient counts
    int global_ingredients_size;
    MPI_Allreduce(&ingredients_size, &global_ingredients_size, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    IngredientCount **global_ingredients = malloc(global_ingredients_size * sizeof(IngredientCount *));
    MPI_Gather(ingredients, ingredients_size, MPI_BYTE, global_ingredients, ingredients_size, MPI_BYTE, 0, MPI_COMM_WORLD);

    // Master process sorts and prints results
    if (rank == 0) {
        sort_ingredients(global_ingredients, global_ingredients_size);

        printf("\nIngredientes após a ordenação:\n");
        print_biggest_three_ingredients(global_ingredients);

        for (int i = 0; i < global_ingredients_size; i++) {
            free(global_ingredients[i]->value);
            free(global_ingredients[i]);
        }
        free(global_ingredients);
    }

    for (int i = 0; i < ingredients_size; i++) {
        free(ingredients[i]->value);
        free(ingredients[i]);
    }
    free(ingredients);

    MPI_Finalize();
    return 0;
}
