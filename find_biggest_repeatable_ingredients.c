#include "./helpers/ingredients.h"
#define filename "test_dataset02.csv"

int main(int argc, char *argv[]) {
    int rank, size;
    long chunk_size, start_offset, end_offset;
    size_t ingredients_size = 0;
    size_t allocated_size = 100; // Alocação inicial para 100 ingredientes
    MPI_Offset file_size;
    MPI_File file;
    MPI_Status status;

    // Inicializa a lista de ingredientes
    IngredientCount **ingredients = malloc(allocated_size * sizeof(IngredientCount *));
    if (ingredients == NULL) {
        printf("Erro ao alocar memória para ingredientes!\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Abre o arquivo MPI
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
    if (!file) {
        printf("Erro ao abrir o arquivo de receitas.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_File_get_size(file, &file_size);

    chunk_size = file_size / size;
    start_offset = rank * chunk_size;
    end_offset = (rank == size - 1) ? file_size : start_offset + chunk_size;

    char *buffer = malloc(chunk_size + 1);
    if (buffer == NULL) {
        printf("Erro ao alocar memória para o buffer.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_File_read_at(file, start_offset, buffer, end_offset - start_offset, MPI_CHAR, &status);
    buffer[end_offset - start_offset] = '\0';

    // Processa os ingredientes linha por linha
    char *line = strtok(buffer, "\n");
    while (line != NULL) {
        // Chama extract_ingredients para processar os ingredientes da linha
        extract_ingredients(line, &ingredients, &ingredients_size, &allocated_size);
        line = strtok(NULL, "\n");
    }


    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        // Ordena os ingredientes e imprime os maiores
        sort_ingredients(ingredients, ingredients_size);
        printf("\nIngredientes após a ordenação:\n");
        print_biggest_three_ingredients(ingredients);
    }

    // Libera recursos
    free(buffer);
    MPI_File_close(&file);

    for (size_t i = 0; i < ingredients_size; i++) {
        free(ingredients[i]->value);
        free(ingredients[i]);
    }
    free(ingredients);

    MPI_Finalize();
    return 0;
}
