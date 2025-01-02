#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INGREDIENTS 10000
#define MAX_LENGTH 100

// Função para dividir a string em uma lista de ingredientes
void split_ingredients(char *str, char ingredients[][MAX_LENGTH], int *count) {
    char *token = strtok(str, ",[]\" ");
    *count = 0;
    while (token != NULL) {
        strcpy(ingredients[*count], token);
        (*count)++;
        token = strtok(NULL, ",[]\" ");
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    FILE *file;
    char line[1024];
    char ingredients[MAX_INGREDIENTS][MAX_LENGTH];
    int ingredient_counts[MAX_INGREDIENTS] = {0};
    int local_counts[MAX_INGREDIENTS] = {0};
    int total_counts[MAX_INGREDIENTS] = {0};
    char unique_ingredients[MAX_INGREDIENTS][MAX_LENGTH];
    int unique_count = 0;

    if (rank == 0) {
        // Processo mestre lê o arquivo CSV
        file = fopen("test_dataset.csv", "r");
        if (!file) {
            printf("Erro ao abrir o arquivo.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Pular o cabeçalho
        fgets(line, sizeof(line), file);
    }

    // Dividir as linhas entre os processos
    int lines_per_process;
    if (rank == 0) {
        int total_lines = 0;
        while (fgets(line, sizeof(line), file)) {
            total_lines++;
        }
        lines_per_process = total_lines / size;
        rewind(file);
        fgets(line, sizeof(line), file); // Pular o cabeçalho novamente
    }

    MPI_Bcast(&lines_per_process, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada processo lê suas linhas atribuídas
    for (int i = 0; i < lines_per_process; i++) {
        if (rank == 0) {
            fgets(line, sizeof(line), file);
            MPI_Send(line, sizeof(line), MPI_CHAR, i % size, 0, MPI_COMM_WORLD);
        } else {
            MPI_Recv(line, sizeof(line), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        int count;
        split_ingredients(line, ingredients, &count);
        for (int j = 0; j < count; j++) {
            int found = 0;
            for (int k = 0; k < unique_count; k++) {
                if (strcmp(ingredients[j], unique_ingredients[k]) == 0) {
                    local_counts[k]++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(unique_ingredients[unique_count], ingredients[j]);
                local_counts[unique_count]++;
                unique_count++;
            }
        }
    }

    // Reduzir os contadores locais para o processo mestre
    MPI_Reduce(local_counts, total_counts, MAX_INGREDIENTS, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Identificar os três ingredientes mais frequentes
        int top_counts[3] = {0};
        char top_ingredients[3][MAX_LENGTH];

        for (int i = 0; i < unique_count; i++) {
            for (int j = 0; j < 3; j++) {
                if (total_counts[i] > top_counts[j]) {
                    for (int k = 2; k > j; k--) {
                        top_counts[k] = top_counts[k - 1];
                        strcpy(top_ingredients[k], top_ingredients[k - 1]);
                    }
                    top_counts[j] = total_counts[i];
                    strcpy(top_ingredients[j], unique_ingredients[i]);
                    break;
                }
            }
        }

        // Imprimir os três ingredientes mais frequentes
        for (int i = 0; i < 3; i++) {
            printf("%s: %d\n", top_ingredients[i], top_counts[i]);
        }
    }

    if (rank == 0) {
        fclose(file);
    }

    MPI_Finalize();
    return 0;
}
