#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_LINE_LENGTH 1024

// Função para contar o ingrediente específico nas linhas do arquivo
int count_ingredient_in_lines(FILE *file, const char *ingredient, int start_line, int end_line) {
    char line[MAX_LINE_LENGTH];
    int count = 0;

    // Pular as linhas até chegar à linha inicial
    for (int i = 0; i < start_line; i++) {
        if (fgets(line, MAX_LINE_LENGTH, file) == NULL) {
            return 0; // Caso as linhas estejam fora do alcance
        }
    }

    // Contar as aparições do ingrediente entre as linhas start_line e end_line
    for (int i = start_line; i < end_line; i++) {
        if (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
            if (strstr(line, ingredient) != NULL) {
                count++;
            }
        }
    }

    return count;
}

int main(int argc, char *argv[]) {
    int rank, size;
    FILE *file;
    const char *filename = "full_dataset.csv";
    const char *ingredient = "chocolate";  // Ingrediente a ser contado
    int total_lines = 0;
    int lines_per_process, start_line, end_line;
    int local_count = 0, global_count = 0;

    // Inicializa o MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // O processo mestre conta o número total de linhas no arquivo
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

    // Todos os processos recebem o número total de linhas
    MPI_Bcast(&total_lines, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Divide o trabalho entre os processos
    lines_per_process = total_lines / size;
    start_line = rank * lines_per_process;
    end_line = (rank == size - 1) ? total_lines : start_line + lines_per_process;

    // Cada processo abre o arquivo novamente para contar as linhas
    file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de receitas.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Cada processo conta as ocorrências do ingrediente em seu intervalo de linhas
    local_count = count_ingredient_in_lines(file, ingredient, start_line, end_line);

    fclose(file);

    // Todos os processos agora fazem a redução de seus contadores locais para obter o total global
    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Sincroniza os processos para garantir que todos terminaram de processar antes de começar a impressão
    MPI_Barrier(MPI_COMM_WORLD);

    // Agora, somente o processo mestre (rank 0) imprime os resultados
    if (rank == 0) {
        // Exibe as mensagens dos processos na ordem correta
        for (int i = 0; i < size; i++) {
            // Cada processo faz uma impressão separada
            printf("Processo %d de %d está processando as linhas %d a %d\n", i, size, i * lines_per_process, (i == size - 1) ? total_lines : (i + 1) * lines_per_process);
        }

        printf("O ingrediente '%s' apareceu %d vezes no arquivo.\n", ingredient, global_count);
    }

    // Finaliza o MPI
    MPI_Finalize();
    return 0;
}
