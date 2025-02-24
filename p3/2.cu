#include <cuda.h>
#include <stdio.h>

__global__ void sumKernel(int *d_arr, int *d_sum, int n) {
  extern __shared__ int sdata[];
  unsigned int tid = threadIdx.x;
  unsigned int i = blockIdx.x * blockDim.x + tid;

  sdata[tid] = (i < n) ? d_arr[i] : 0;
  __syncthreads();

  for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
    if (tid < s) {
      sdata[tid] += sdata[tid + s];
    }
    __syncthreads();
  }

  if (tid == 0) {
    atomicAdd(d_sum, sdata[0]);
  }
}

int main() {
  int n;
  scanf("%d", &n);

  int size = 1 << n;  // 2^n
  int *h_arr = (int *)malloc(size * sizeof(int));
  int h_sum = 0;

  for (int i = 0; i < size; i++) {
    h_arr[i] = i;
  }

  int *d_arr, *d_sum;
  cudaMalloc((void **)&d_arr, size * sizeof(int));
  cudaMalloc((void **)&d_sum, sizeof(int));
  cudaMemcpy(d_arr, h_arr, size * sizeof(int), cudaMemcpyHostToDevice);

  int threadsPerBlock = 1024;
  int blocksPerGrid = (size + threadsPerBlock - 1) / threadsPerBlock;

  sumKernel<<<blocksPerGrid, threadsPerBlock, threadsPerBlock * sizeof(int)>>>(
      d_arr, d_sum, size);

  cudaDeviceSynchronize();

  cudaMemcpy(&h_sum, d_sum, sizeof(int), cudaMemcpyDeviceToHost);

  printf("%d\n", h_sum);

  cudaFree(d_arr);
  cudaFree(d_sum);
  free(h_arr);

  return 0;
}
