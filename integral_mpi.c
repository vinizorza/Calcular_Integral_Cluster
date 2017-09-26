#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


double integral = 0.0;

typedef struct
{
    double a;
    double b;
    int N;
} Targumentos;

double AreaTrapezio(double dx, double h1, double h2)
{
    return (dx*(h1+h2)/2);
}

double f(double x)
{	
    return (4*sqrt(1-x*x));
}

double ThreadCalculaArea(Targumentos *argumentos)
{
    int i;
    double area, dx, x1, x2, f1, f2;
    Targumentos arg;
    arg = *argumentos;
    double a=arg.a; double b=arg.b; int N=arg.N;
    printf("a=%10.1lf\tb=%10.1lf\tN=%d\n", a, b, N);
    dx   = (b-a)/N;
    area = 0.0;
    for (i=0; i<N; i++)
    {
        x1 = a + dx * i;
        x2 = a + dx * (i+1);
        f1 = f(x1);
        f2 = f(x2);
        area += AreaTrapezio(dx, f1, f2);
    }
    integral += area;
    return integral;
}


int main(int argc, char *argv[]){
	
	int rank, size;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	
	double a, b;
    int N;
    Targumentos arg1, arg2, arg3, arg4;
    if (argc<4)
    {
        printf("Numero de argumentos insuficiente...\n");
        exit(-1);
    }
    a = (double) atof(argv[1]);
    b = (double) atof(argv[2]);
    N = atoi(argv[3]);

    /* arg1 */
    arg1.a = a,
    arg1.b = (a+b)/4,
    arg1.N = N/4;
    /* arg2 */
    arg2.a = (a+b)/4,
    arg2.b = ((a+b)/4)*2,
    arg2.N = N/4;
    /* arg3 */
    arg3.a = ((a+b)/4)*2,
    arg3.b = ((a+b)/4)*3,
    arg3.N = N/4;
    /* arg4 */
    arg4.a = ((a+b)/4)*3,
    arg4.b = b,
    arg4.N = N/4;

	double total = 0;
	
	
	if(rank == 0){
		total += ThreadCalculaArea(&arg1);
		for(int i = 1; i <= 3; i++){
			MPI_Recv(&integral, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			total += integral;
		}		
	}else if(rank == 1){
		integral = ThreadCalculaArea(&arg2);
		MPI_Send(&integral,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);				
	}else if(rank == 2){
		ThreadCalculaArea(&arg3);
		MPI_Send(&integral,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);	
	}else if(rank == 3){
		ThreadCalculaArea(&arg4);
		MPI_Send(&integral,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);		
	}	
	
	if(rank == 0){
		printf("\nResultado: [%f]\n",total);
	}
	


	MPI_Finalize();


}
