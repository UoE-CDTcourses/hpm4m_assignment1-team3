#include <iostream>
#include <stdlib.h> 
#include <mpi.h>
#include <time.h>

using namespace std;

int main(){

  int rank, size, ierr;
  MPI_Comm comm;

  comm  = MPI_COMM_WORLD;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(comm, &rank);            
  MPI_Comm_size(comm, &size);
  
  MPI_Request request;   // for non-blicking send-receive later on
  MPI_Status status;

  int N = size;
  int A[N][N];
  int B[N][N];
  int C[N];
  int D[N][N];

// root process generates matrix B 
  if (rank == 0) {
     for (int i = 0; i < N; i++) {
         int ii = i + 1;
         for (int j = 0; j < N; j++) {   
             int jj = j + 1;
            B[i][j] = (jj+ii) * (N-jj+1);
          }
      }
  }   

// Broadcast matrix data to the other processes
  MPI_Bcast(&B, N*N, MPI_INT, 0 , comm);

// Each process creates their corresponding row of matrix A 
  for (int j = 0; j < N; j++) {   
      int jj = j + 1;
      int rr = rank + 1;
      A[rank][j] = (N-jj+rr+1) * rr;
  }
  
// Each process multiplies their row of A with the entire row of B
// and  stores the answer in vector c.
  for (int j = 0; j < N; j++) {
      C[j] = 0;
      for (int k = 0; k < N; k++) {
          C[j] += A[rank][k] * B[k][j];
      }
  }
 // Root process gathers all vectors C from each process and puts them
 // into matrix D 
  MPI_Gather(&C, N, MPI_INT, &D, N, MPI_INT, 0, comm);

  if (rank == 0) {
     for (int i = 0; i < N; i++) {
         for (int j = 0; j < N; j++) {   
             cout << "(i,j)= "<< "(" <<i << ","<< j << ")" << "  and  " << "D[i][j]= "<< D[i][j] << endl;
         }
     }
  }


  MPI_Finalize();


}
