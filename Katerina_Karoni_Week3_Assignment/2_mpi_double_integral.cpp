#include <iostream>
#include <stdlib.h> 
#include <mpi.h>
#include <time.h>
#include <math.h>

using namespace std;
template <typename func_type>

double trapezoid(double a, double b, int n, func_type f, int rank, int size)
{
  double dx   = a/n;
  double dy   = b/n;
  double sum1 = 0.0;      //sum_[f(xi,0) + f(xi,b)]
  double sum2 = 0.0;      //sum_[f(0,yj)+ f(a,yj)]
  double sum3 = 0.0;      //sum_f(xi,yj)

  for (int i = 1+rank; i<n; i+=size){
      sum1 += f(i*dx,0.0) + f(i*dx,b);
  }    
  sum1 = 2.0 * sum1;

  for (int j = 1+rank; j<n; j+=size){
      sum2 += f(0.0,j*dy)+f(a,j*dy);
  }
  sum2 = 2.0 * sum2;

  for (int i = 1+rank; i<n; i+=size){
      for (int j = 1; j<n; j++){
          sum3 += f(i*dx,j*dy);
      }
  }
  sum3 = 4.0 * sum3;

  return (sum1 + sum2 + sum3); 

}

double f(double x, double y)
{ 
      double z = x*sin(x*x) + y*sin(y*y);
//      double z = x*sin(pow(x,x)) + y*sin(pow(y,y));
  return z;
}

int main()
{

  int rank, size, ierr;
  MPI_Comm comm;

  comm  = MPI_COMM_WORLD;

  MPI_Init(NULL,NULL);
  MPI_Comm_rank(comm, &rank);            
  MPI_Comm_size(comm, &size);
  MPI_Request request;   // for non-blicking send-receive later on
  MPI_Status status;

  double a   = 100;
  double b   = 100;
  int n      = 100000;
  double dx  = a / n;
  double dy  = b / n;
  double res = trapezoid(a, b, n ,f,rank,size);
  double sum123; 

  MPI_Reduce(&res,&sum123, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
  double integral_appr = (dx*dy/4)*(f(0.0,0.0) +f(a,0.0) +f(0.0,b)+f(a,b) + sum123);  

  if (rank == 0){
     cout << "trapezoid integral approximation = "<< integral_appr << endl;
     cout << "analytical solution = "<< 0.5 * (-b*cos(a*a)-a*cos(b*b)+ a + b)  << endl;
  }

  MPI_Finalize();

}
