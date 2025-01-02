#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_STRING_LENGTH 256

// Função para limpar a coluna NER (remover colchetes e aspas)
void clean_ner_column(char *ner_column) {
    char *start = ner_column;
    // Remover os espaços e aspas no início
    while (*start == '"' || *start == '[' || *start == ' '){
        start++;
    }

    char *end = start + strlen(start) - 1;
    // Remover os espaços e aspas no final
    while (*end == '"' || *end == ']' || *end == ' '){
        *end = '\0';
        end--;
    }

    // Agora, separar os ingredientes com vírgulas
    char cleaned[MAX_STRING_LENGTH] = "";
    char *token = strtok(start, ",");  // Separar por vírgulas
    while (token != NULL) {
        // Remover espaços extras antes e depois do ingrediente
        while (*token == ' '){
            token++;
        }
        strcat(cleaned, token);  // Adicionar ingrediente à string limpa
        strcat(cleaned, ", ");  // Adicionar vírgula entre ingredientes
        token = strtok(NULL, ",");  // Continuar separando
    }

    // Remover a vírgula final
    if (strlen(cleaned) > 0) {
        cleaned[strlen(cleaned) - 2] = '\0';  // Remove a última vírgula e espaço
    }

    // Atualiza o ner_column com a versão limpa
    strcpy(ner_column, cleaned);
}

int main(int argc, char *argv[]) {
    FILE *file;
    char line[MAX_LINE_LENGTH], *token, *saveptr;

    // Variáveis para armazenar cada coluna
    char ner_column[MAX_STRING_LENGTH];

    file = fopen("test_dataset.csv", "r");

    if (!file) {
        printf("Error opening CSV file...\n");
        return 1;
    }

    // Pular o cabeçalho (linha 0)
    fgets(line, MAX_LINE_LENGTH, file);

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // Tokenizar a linha para separar as colunas
        token = strtok_r(line, ",", &saveptr);  // Pular a primeira coluna (title)
        token = strtok_r(NULL, ",", &saveptr);  // Pular a segunda coluna (ingredients)
        token = strtok_r(NULL, ",", &saveptr);  // Pular a terceira coluna (directions)
        token = strtok_r(NULL, ",", &saveptr);  // Pular a quarta coluna (link)
        token = strtok_r(NULL, ",", &saveptr);  // Pular a quinta coluna (source)

        // Agora pega a última coluna (NER)
        if (token) {
            strcpy(ner_column, token);
            clean_ner_column(ner_column);  // Limpar a coluna NER
        }

        // Imprimir somente a coluna NER limpa (ingredientes)
        printf("[%s]\n", ner_column);  // Exibe a lista de ingredientes limpa

    }

    fclose(file);
    return 0;
}
