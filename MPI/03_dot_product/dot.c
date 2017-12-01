#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void read_array(double ** local_x, int local_n, int global_n, int rank, int num_procs, char input_filename[]){
    *local_x=(double *)malloc(local_n*sizeof(double));
    MPI_Status * status;

    if(rank == num_procs-1){
        printf ("Reading data with proc. rank %d and distributing data to other procs\n",rank);
        FILE *infile=fopen(input_filename,"r");
        for(int iproc=0; iproc<num_procs; iproc++){
            for(int iloc=0; iloc<local_n; iloc++){
                double temp;
                fscanf(infile, "%lf\n", &temp);
                (*local_x)[iloc] = temp;
            }
            if(iproc < num_procs-1){
                int tag=iproc;
                MPI_Send((*local_x),local_n,MPI_DOUBLE,iproc,tag,MPI_COMM_WORLD);
            }
        }
        fclose(infile);
        printf("Data readin complete\n");
    }else{
        int tag=rank;
        MPI_Recv((*local_x),local_n,MPI_DOUBLE,num_procs-1,tag,MPI_COMM_WORLD,status);
    }
}

double serial_dot(double x[], double y[], int n){
    double sum=0.0;
    for(int i=0; i<n; i++){
        sum += x[i]*y[i];
    }
    return sum;
}

double parallel_dot(double local_x[], double local_y[], int local_n){
    double local_dot = serial_dot(local_x, local_y, local_n);
    double global_dot = 0.0;
    MPI_Reduce(&local_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    return global_dot;
}

int main(int argc, char * argv[]){
    int num_procs,my_rank;
    MPI_Init     (&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(argc != 4){
        printf("usage: %s inputfile1 inputfile2 global_n\n",argv[0]);
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    
    int global_n=atoi(argv[3]);
    if(global_n % num_procs != 0){
        printf("ERROR: global_n mod num_procs != 0\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    int local_n=global_n/num_procs;
    MPI_Barrier(MPI_COMM_WORLD);
    
    double * local_x;
    double * local_y;
    read_array(&local_x,local_n,global_n,my_rank,num_procs,argv[1]);
    read_array(&local_y,local_n,global_n,my_rank,num_procs,argv[2]);

    double t1;
    t1 = MPI_Wtime(); 
    double dot  = parallel_dot(local_x,local_y,local_n);
    if(my_rank == num_procs-1){
        printf("Calculation took %f second\n", MPI_Wtime()-t1); 
    }
    
    if(my_rank==0){
        printf("Dor product : %f\n",dot);
    }

    MPI_Finalize();
    free(local_x);    
}
