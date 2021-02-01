#include <iostream> //includes packages
#include <math.h>
#include <string>
#include <mpi.h>

using namespace std; //using standard function names.

//Predefining the function to integrate.
double f(double x,double y)
{ double a = x*sin(x*x) + y*sin(y*y);
  return a;
}

int main(void)
{
  float a;
  float b;
  int N_x;
  int N_y;
  float int_approx;
  float int_exact;
  float numerical_error;




  a = 100;
  b = 100;
  N_x = 100000;
  N_y = 100000;

  double h_x;
  double h_y;
  int i,j;
  double sum;
  double finalsum;
  int rank, ierr, size;
  MPI_Comm comm;
  const int root = 0;
  comm = MPI_COMM_WORLD;
  MPI_Init(NULL,NULL);
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);


  h_x = a/ N_x;
  h_y = b/ N_y;

  sum = 0.0;

  for (i = rank+1; i<N_x; i+=size)
  {
    sum += 2*(f(i*h_x,0) + f(i*h_x,b));
  }
  for (i = rank+1; i<N_y; i+=size){

    sum += 2*(f(0,i*h_y) + f(a,i*h_y));
  }
  for (i = rank+1; i<N_x; i+=size)
  { for (j=1;j<N_y;j++){
    sum += 4*f(i*h_x,j*h_y);
  }
  }
  //Reducing to a final sum.
  MPI_Reduce(&sum,&finalsum,1,MPI_DOUBLE,MPI_SUM,root,comm);
  if (rank == root){




    int_approx = (finalsum + f(0,0) + f(a,0) + f(a,b) + f(0,b)) * h_x * h_y / 4;
    cout <<"The approximate integral = " <<int_approx << endl;

    int_exact = 0.5*(-b*cos(a*a) - a*cos(b*b) + a + b);
    numerical_error = abs(int_exact-int_approx);
    cout <<"The numerical error = " << numerical_error << endl;

  }
  MPI_Finalize();
}
