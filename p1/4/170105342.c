#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define MAX 1000

int main(int argc, char **argv) {
  char texto_base[] =
      "abcdefghijklmnopqrstuvwxyz 1234567890 ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int texto_tamanho = strlen(texto_base);

  int rank, size, cont = 0;
  MPI_Init(&argc, &argv);                // Inicializa o ambiente MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Obtém o rank do processo
  MPI_Comm_size(MPI_COMM_WORLD, &size);  // Obtém o número total de processos

  if (size != 3) {  // Verifica se o número de processos é 3
    MPI_Finalize();
    return 1;
  }

  struct timeval tv;
  int number, local_cont = 0, indice = 0;

  while (cont < MAX) {  // Loop principal que executa até cont ser igual a MAX
    if (rank == 0) {  // Apenas o processo com rank 0 calcula o valor de number
      gettimeofday(&tv, NULL);
      number = ((tv.tv_usec / 47) % 3) + 1;
    }

    // O valor de number é transmitido para todos os processos
    MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada processo calcula seu intervalo de trabalho
    int inicio = (rank * texto_tamanho) / 3 + indice;
    int fim = ((rank + 1) * texto_tamanho) / 3;

    int i;
    for (i = 0; i < number; i++) {  // Cada processo imprime sua parte do texto
      int pos = inicio + i;
      if (pos >= texto_tamanho) {
        break;
      }
      fprintf(stderr, "%c", texto_base[pos]);
    }

    local_cont += i;  // Atualiza o contador local
    indice += i;      // Atualiza o índice

    if (indice >= texto_tamanho) {  // Se o índice ultrapassar o tamanho do
                                    // texto, reinicia
      if (rank == 0) {
        fprintf(stderr, "\n");
      }
      indice = 0;
      cont++;
    }

    // Todos os processos atualizam a variável cont de forma colaborativa
    MPI_Allreduce(&local_cont, &cont, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  }

  MPI_Finalize();  // Finaliza o ambiente MPI
  return 0;
}
