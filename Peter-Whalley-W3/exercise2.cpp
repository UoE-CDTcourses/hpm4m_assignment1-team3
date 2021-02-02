#include <iostream> //includes packages
#include <mpi.h>
using namespace std;
int main()
{
  int rank, ierr, size;
  MPI_Comm comm;
  comm = MPI_COMM_WORLD;
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);
  int i, j, k, l, m, n, o;
  const int root = 0;
  int N = size;
  int A[N];
  int B[N][N];
  int D[N][N];
  int C[N];
  if (rank == root) {
    for (i=0; i < N; i++)
    {
      for (j=0; j < N; j++)
      {
      B[i][j] = ((j+1) + (i+1))*(N-(j+1)+1);
      }
    }
  }
  //Broadcast B
  MPI_Bcast(&B, N*N, MPI_INT,root,comm);

  //Barrier
  MPI_Barrier(comm);

  for (j=0; j < N; j++)
    {
    A[j] = (N-(j+1)+(rank+1)+1)*(rank+1);

  }
  //multiplying each processors row of A by the entire matrix B
  //and storing it as C.
  for (l = 0; l< N; l++)

    { C[l] = 0;
      for  (m = 0; m<N ; m++)
      {
        C[l] += A[m]*B[m][l];
      }
    }
  //Gathering all the rows of D.
  MPI_Gather(&C,N,MPI_INT,&D,N,MPI_INT,root, comm);
  //Barrier
  MPI_Barrier(comm);
  //Reconstruct Matrix.
  //Printing Matrix
  if (rank == root){
  for(n = 0;n<N;n++)
   {
       for(o=0;o<N;o++)
       {
          cout<<D[n][o]<< " ";
       }
   cout<<endl;
   }
 }
  MPI_Finalize();
}
