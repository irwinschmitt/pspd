// Somas paralelas com threads OpenMP
// Dado um valor n de entrada, construa um programa OpenMP que produza como
// saída a soma dos naturais no intervalo de [0 … (2^n)-1].

// O valor n de entrada é sempre o expoente da função 2^n e representa a
// quantidade de números naturais a serem somados, a partir do zero. Por
// exemplo, se n=3, a fórmula 2³, resulta 8 e os valores a serem somados são 0 1
// 2 3 4 5 6 7. O valor de saída refere-se à soma dos números naturais
// identificados pelo parâmetro de entrada O tipo long int é suficiente para
// representar o valor a ser impresso na saída (considere que todos os casos de
// teste produzem somas que cabem numa variável do tipo long int) Entrada O
// arquivo de entrada contém o expoente n>0, da fórmula 2^n

// Saída
// O arquivo de saída contém o valor relativo à soma da lista de números
// naturais do intervalo [0 … (2^n)-1]

// Restrições
// Não são admitidas construções usando #pragma omp for reduction()
// Não são admitidas construções com o uso de pragmas de bloqueio como #pragma
// omp atomic ou #pragma omp critical Para uma determinada entrada, o programa
// deve produzir sempre a mesma saída, independente da quantidade de threads
// instanciadas

#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  int n;
  scanf("%d", &n); // Lê o valor de n (expoente da função 2^n)

  long int limit = (long int)pow(2, n);  // Calcula 2^n, que define o limite superior do intervalo [0, 2^n - 1]
  long int sum = 0; // Variável para armazenar a soma total
  int num_threads; // Variável para armazenar o número de threads

#pragma omp parallel // Inicia uma região paralela com OpenMP
  {
    int id = omp_get_thread_num(); // Obtém o ID da thread atual
    int nthrds = omp_get_num_threads(); // Obtém o número total de threads
    if (id == 0) num_threads = nthrds; // A thread 0 armazena o número total de threads

    long int local_sum = 0; // Variável para armazenar a soma local de cada thread
    // Cada thread calcula a soma de uma parte do intervalo [0, 2^n - 1]
    for (long int i = id; i < limit; i += nthrds) {
      local_sum += i; // Soma os números atribuídos a esta thread
    }

#pragma omp barrier // Sincroniza todas as threads antes de prosseguir

    static long int *partial_sums = NULL; // Ponteiro para armazenar as somas parciais de cada thread
#pragma omp single // Apenas uma thread executa este bloco
    {
      partial_sums = (long int *)malloc(num_threads * sizeof(long int)); // Aloca memória para as somas parciais
      for (int i = 0; i < num_threads; i++) {
        partial_sums[i] = 0; // Inicializa o vetor de somas parciais com 0
      }
    }

    partial_sums[id] = local_sum; // Armazena a soma local da thread no vetor de somas parciais

#pragma omp barrier // Sincroniza todas as threads antes de prosseguir

#pragma omp single // Apenas uma thread executa este bloco
    {
      for (int i = 0; i < num_threads; i++) {
        sum += partial_sums[i]; // Soma todas as somas parciais para obter a soma total
      }
      free(partial_sums); // Libera a memória alocada para as somas parciais
    }
  }

  printf("%ld\n", sum); // Imprime a soma total
  return 0;
}
