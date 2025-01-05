#include "./helpers/ingredients.h"
#define filename "test_dataset02.csv"

int main(int argc, char *argv[]) {
    int rank, size;
    long chunk_size, start_offset, end_offset;
    int ingredients_size ;
    MPI_Offset file_size;
    MPI_File file;
    MPI_Status status;
    IngredientCount **ingredients = malloc(0);
 
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

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

    MPI_File_read_at(file, start_offset, buffer, end_offset - start_offset, MPI_CHAR, &status);
    buffer[end_offset - start_offset] = '\0';

     extract_ingredients(buffer, &ingredients, &ingredients_size);

/*    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        sort_ingredients(ingredients, ingredients_size);
        printf("\nIngredientes após a ordenação:\n");
        print_biggest_three_ingredients(ingredients);
    }

    free(buffer);

    MPI_File_close(&file);

    for (int i = 0; i < ingredients_size; i++) {
        free(ingredients[i]->value);
        free(ingredients[i]);
    }
    free(ingredients); */

    MPI_Finalize();
    return 0;
}