#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int get_local_n(int global_n, int rank, int num_procs){
    /* Calculates local size of the array to store in memory */
    int local_n;
    if(global_n % num_procs == 0){
        local_n = global_n / num_procs;
    }else{
        if(rank != num_procs-1) {local_n = (global_n / num_procs) + 1;             } 
        else                    {local_n = global_n % ((global_n / num_procs) + 1);}
    } 
    return local_n;
}

void read_array(double ** local_x, int global_n, int rank, int num_procs, char input_filename[]){
    /* Uses process num_procs-1 to read in the input from a file and distribute it to the processes */
    int local_n_max = get_local_n(global_n,0,num_procs);
    *local_x=(double *)malloc(local_n_max*sizeof(double));
    MPI_Status * status;

    if(rank == num_procs-1){
        // printf ("Reading data with proc. rank %d and distributing data to other procs\n",rank);
        FILE *infile=fopen(input_filename,"r");
        for(int iproc=0; iproc<num_procs; iproc++){
            int local_n = get_local_n(global_n,iproc,num_procs);
            for(int iloc=0; iloc<local_n; iloc++){
                fscanf(infile, "%lf\n", (*local_x)+iloc);
            }
            if(iproc < num_procs-1){
                int tag=iproc;
                MPI_Send((*local_x),local_n,MPI_DOUBLE,iproc,tag,MPI_COMM_WORLD);
            }
        }
        fclose(infile);
    }else{
        int local_n = get_local_n(global_n,rank,num_procs);
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

double parallel_dot(double local_x[], double local_y[], int global_n, int rank, int num_procs){
    int local_n=get_local_n(global_n,rank,num_procs);
    double local_dot = serial_dot(local_x, local_y, local_n);
    double global_dot = 0.0;
    MPI_Reduce(&local_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, num_procs-1, MPI_COMM_WORLD);
    return global_dot;
}

int main(int argc, char * argv[]){
    int num_procs,rank;
    MPI_Init     (&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(argc != 4){
        printf("usage: %s inputfile1 inputfile2 global_n\n",argv[0]);
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    
    int global_n=atoi(argv[3]);

    MPI_Barrier(MPI_COMM_WORLD);
    
    double * local_x;
    double * local_y;
    read_array(&local_x,global_n,rank,num_procs,argv[1]);
    read_array(&local_y,global_n,rank,num_procs,argv[2]);

    double t1;
    t1 = MPI_Wtime(); 
    double dot  = parallel_dot(local_x,local_y,global_n,rank,num_procs);
    
    if(rank==num_procs-1){
        printf("Num_procs  : %d   |   ",num_procs);
        printf("Calc_time  : %f   |   ",MPI_Wtime()-t1);
        printf("Dot prod   : %f\n"     ,dot);
    }

    MPI_Finalize();
    free(local_x);    
}
