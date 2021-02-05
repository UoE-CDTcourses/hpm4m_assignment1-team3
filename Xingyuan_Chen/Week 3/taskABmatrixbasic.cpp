//
// prints what processor it is out of the total number of processors from all ranks
// and finds the closest multiple of three to its rank
//

#include <iostream>
#include <mpi.h>
using namespace std;

int main(){
  int rank, size, ierr;
  MPI_Comm comm;

  comm  = MPI_COMM_WORLD;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(comm, &rank);            
  MPI_Comm_size(comm, &size);
  
  // Find the closest multiple of three to your rank below ...
  
  // Each process prints out its rank
  //cout << "I am "<<rank<<" out of "<<size<<" and closest multiple of 3 to me is ..."<<endl;
  
  int matrixB[size][size], i, j;
  if (rank == 0) {
	  
	  //make the matrix B
	  for (i = 0; i < size; i++) {
		  for (j = 0; j < size; j++) {
			  matrixB[i][j] = (j + i+2) * (size - j -1+ 1);
		  }
	  }
	  // send matrix B to every core
	  for (i = 0; i < size; i++) {
		  MPI_Send(&(matrixB[0][0]), size * size, MPI_INT, i, -999, MPI_COMM_WORLD);
	  }

  }

  // receive matrix B from core 0.
  if (rank != 0) {
	  MPI_Recv(&(matrixB[0][0]), size * size, MPI_INT, 0, -999, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  
  //check if everything works as expected by ouput first element of B
  //cout << "I am " << rank << " " <<matrixB[0][0] << endl;

  int matrixAB[size][size];
  int rowA[size], row;
  int rowAB[size];

  //row of matrix A, is the (rank+1) row, start from 0.
  for (j = 0; j < size; j++) {
	  rowA[j] = (size-j+rank+1) * (rank + 1);
	  rowAB[j] = 0;
  }

  for (j = 0; j < size; j++) {
	  for (i = 0; i < size; i++) {
		  rowAB[j] += rowA[i] * matrixB[i][j];

	  }
  }
  
  // check if core calculate AB row right. : It is right!
  /*
  cout << "I am " << rank << " ";
  for (j = 0; j < size; j++) {
	  cout << rowAB[j] << " ";
  }
  cout << endl;
*/


  //core 0 safe his own A*B result.
  if (rank == 0) {
	  for (i = 0; i < size; i++) {
		  matrixAB[0][i] = rowAB[i];
	  }
  }
  
  // other core send rowAB
  if (rank!=0) {
	  MPI_Send(&(rowAB[0]), size , MPI_INT, 0, rank, MPI_COMM_WORLD);
  }

  // core 0 receive and safe A*B:
  if (rank == 0) {
	  for (i = 1; i < size; i++) {
		  MPI_Recv(&(rowAB[0]), size, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		  for (j = 0; j < size; j++) {
			  matrixAB[i][j] = rowAB[j];
		  }
	  }
  }
  
  //print the matrixAB.
  if (rank == 0) {
	  for (i = 0; i < size; i++) {
		  for (j = 0; j < size; j++) {
			  cout << matrixAB[i][j] << " ";
		  }
		  cout << endl;
	  }
  }
 



  MPI_Finalize();

}
