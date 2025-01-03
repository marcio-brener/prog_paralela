#include "ingredients.h"

#define MAX_LINE_LENGTH 1024

int main(int argc, char *argv[]) {
    int rank, size;
    FILE *file;
    const char *filename = "test_dataset02.csv";
    const char *ingredient = "vanila";
    int total_lines = 0;
    int lines_per_process, start_line, end_line;
    int local_count = 0, global_count = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        file = fopen(filename, "r");
        if (!file) {
            printf("Erro ao abrir o arquivo de receitas.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        char line[MAX_LINE_LENGTH];
        while (fgets(line, MAX_LINE_LENGTH, file)) {
            total_lines++;
        }
        fclose(file);
    }

    MPI_Bcast(&total_lines, 1, MPI_INT, 0, MPI_COMM_WORLD);

    lines_per_process = total_lines / size;
    start_line = rank * lines_per_process;
    end_line = (rank == size - 1) ? total_lines : start_line + lines_per_process;

    file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de receitas.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    local_count = count_ingredient_in_lines(file, ingredient, start_line, end_line);

    fclose(file);

    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            printf("Processo %d de %d está processando as linhas %d a %d\n", i, size, i * lines_per_process, (i == size - 1) ? total_lines : (i + 1) * lines_per_process);
        }

        printf("O ingrediente '%s' apareceu %d vezes no arquivo.\n", ingredient, global_count);
    }

    MPI_Finalize();
    return 0;
}