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
  //rank start from 0
  // set N here !!!!

  int N = 10;
  int matrixB[N][N], i, j;
  
  
  //if (rank >= N) { goto label01; } //rank start from 0.
  


  if (rank == 0) {
	  
	  //make the matrix B
	  for (i = 0; i < N; i++) {
		  for (j = 0; j < N; j++) {
			  matrixB[i][j] = (j + i+2) * (N- j -1+ 1);
		  }
	  }
	  // send matrix B to every core
	  for (i = 0; i < size; i++) {
		  MPI_Send(&(matrixB[0][0]), N * N, MPI_INT, i, -999, MPI_COMM_WORLD);
	  }

  }

  // receive matrix B from core 0.
  if (rank != 0) {
	  MPI_Recv(&(matrixB[0][0]), N * N, MPI_INT, 0, -999, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  

  int matrixAB[N][N];
  int rowA[N], row;
  int rowAB[N];

  int caltimes,fz,kk,Acol;//Acol is Arow, wrong name! XD

  // know the calculation times of this core:
  caltimes = 0;
  caltimes = N / size;
  if ((N % size - 1) >= rank) { caltimes += 1;  }

  cout << "I am " << rank << "  my calculate times are " << caltimes;
  //each core needs to calculate caltimes.

  for (kk = 0; kk < caltimes; kk++) {

	  Acol = rank + kk * size;
	  for (j = 0; j < N; j++) {
		  rowA[j] = (N - j + Acol + 1) * (Acol + 1);
		  rowAB[j] = 0;
	  }

	  for (j = 0; j < N; j++) {
		  for (i = 0; i < N; i++) {
			  rowAB[j] += rowA[i] * matrixB[i][j];

		  }
	  }
	  //check if the rowAB result
	  /*
	  cout << "I am " << rank << "  is calculating row "<<Acol;
	  for (j = 0; j < N; j++) {
		  cout << rowAB[j] << " ";
	  }
	  cout << endl;
	  */

	  // other core send rowAB
	  if (rank != 0) {
		  MPI_Send(&(rowAB[0]), N, MPI_INT, 0, Acol, MPI_COMM_WORLD);
	  }
	  //core 0 safe his own A*B result.
	  if (rank == 0) {
		  for (i = 0; i < N; i++) {
			  matrixAB[Acol][i] = rowAB[i];
		  }
	  }
  }
  

  //row of matrix A, is the (rank+1) row, start from 0.
 
  
  // check if core calculate AB row right. : It is right!
  /*
  cout << "I am " << rank << " ";
  for (j = 0; j < size; j++) {
	  cout << rowAB[j] << " ";
  }
  cout << endl;
*/



  


  // core 0 receive and save A*B:
  if (rank == 0) {
	  for (i = 1; i < N; i++) {
		  if ((i) % size != 0) {      // receive from others but not himself part.
			  MPI_Recv(&(rowAB[0]), N, MPI_INT, (i) % size, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			  //cout <<"Receive col"<< i <<" from "<<i%size<<" FN "<< rowAB[0] << " "<<endl;
			  for (j = 0; j < N; j++) {
				  matrixAB[i][j] = rowAB[j];
			  }

		  }  
	  }
  }
  
  //print the matrixAB.
  if (rank == 0) {
	  for (i = 0; i < N; i++) {
		  for (j = 0; j < N; j++) {
			  cout << matrixAB[i][j] << " ";
		  }
		  cout << endl;
	  }
  }
 
//label01:;


  MPI_Finalize();

}
