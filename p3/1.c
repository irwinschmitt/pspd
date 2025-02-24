
// Cálculo colaborativo da soma de uma lista de números
// Dada uma lista de números de entrada, produza a soma desses elementos por meio de um programa MPI, considerando o seguinte:

// A lista de números deve ser dividida uniformemente entre os 5 processos workers MPI gerados em tempo de execução, incluindo o MASTER. Por exemplo, se a quantidade de números da lista é igual a 36 e o programa for instanciado com 5 processos, quatro processos deverão tratar um subgrupos de 7 números e um deles deverá tratar um subgrupo de 8 números.
// Entrada
// Na linha do arquivo de entrada o primeiro número refere=se à quantidade de números a serem trabalhados. Os demais números do arquivo de entrada é a lista de números a serem somados

// Saída
// O arquivo de saída contém o valor relativo à soma da lista de números do arquivo de entrada

// Restrições
// Na parte MPI do programa, usar apenas as primitivas MPI_Send, MPI_Recv para comunicação entre Master e Slaves

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int rank, size, n, i;
  int *numbers = NULL;          // Ponteiro para armazenar a lista completa de números
  int local_sum = 0, total_sum = 0; // Variáveis para armazenar a soma local e total
  int *sub_numbers = NULL;      // Ponteiro para armazenar o subconjunto de números de cada processo
  int sub_size;                 // Tamanho do subconjunto de números de cada processo

  // Inicializa o ambiente MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtém o rank (identificador) do processo atual
  MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtém o número total de processos

  // O processo MASTER (rank 0) é responsável por ler a entrada e distribuir os dados
  if (rank == 0) {
    scanf("%d", &n); // Lê o número total de elementos na lista
    numbers = (int *)malloc(n * sizeof(int)); // Aloca memória para a lista completa
    for (i = 0; i < n; i++) {
      scanf("%d", &numbers[i]); // Lê cada número da lista
    }

    // Distribui os números entre os processos workers
    int offset = 0; // Offset para controlar a posição inicial de cada subconjunto
    for (i = 1; i < size; i++) {
      int count = n / size + (i < n % size ? 1 : 0); // Calcula o tamanho do subconjunto
      MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // Envia o tamanho do subconjunto
      MPI_Send(numbers + offset, count, MPI_INT, i, 0, MPI_COMM_WORLD); // Envia o subconjunto
      offset += count; // Atualiza o offset para o próximo subconjunto
    }
    // O MASTER também processa um subconjunto
    sub_size = n / size + (0 < n % size ? 1 : 0); // Calcula o tamanho do subconjunto do MASTER
    sub_numbers = numbers + offset; // Define o subconjunto do MASTER
  } else {
    // Os processos workers recebem o tamanho do subconjunto e os números
    MPI_Recv(&sub_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    sub_numbers = (int *)malloc(sub_size * sizeof(int)); // Aloca memória para o subconjunto
    MPI_Recv(sub_numbers, sub_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  // Cada processo calcula a soma local do seu subconjunto
  for (i = 0; i < sub_size; i++) {
    local_sum += sub_numbers[i];
  }

  // O MASTER coleta as somas parciais dos workers e calcula a soma total
  if (rank == 0) {
    total_sum = local_sum; // Inicializa a soma total com a soma local do MASTER
    for (i = 1; i < size; i++) {
      int partial_sum;
      MPI_Recv(&partial_sum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      total_sum += partial_sum; // Adiciona a soma parcial de cada worker à soma total
    }

    printf("%d\n", total_sum); // Imprime a soma total
    free(numbers); // Libera a memória alocada para a lista completa
  } else {
    // Os workers enviam suas somas parciais para o MASTER
    MPI_Send(&local_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    free(sub_numbers); // Libera a memória alocada para o subconjunto
  }

  // Finaliza o ambiente MPI
  MPI_Finalize();
  return 0;
}
