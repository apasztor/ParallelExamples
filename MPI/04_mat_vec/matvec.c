#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
/* matrix times vector */
/* M : n X m */
/* v : m */

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

void read_array_serial(double **v, int m, char input_filename[]){
    *v=(double *)malloc(m*sizeof(double));
    FILE *infile=fopen(input_filename,"r");
    for(int i=0; i<m; i++){
        fscanf(infile, "%lf\n", (*v)+i);
    }
    fclose(infile);
}

void read_matrix_parallel(double ** local_x, int global_n, int m, int rank, int num_procs, char input_filename[]){
    /* Uses process num_procs-1 to read in the input from a file and distribute it to the processes */
    int local_n_max = get_local_n(global_n,0,num_procs);
    *local_x=(double *)malloc(local_n_max*m*sizeof(double));
    MPI_Status * status;

    if(rank == num_procs-1){
        // printf ("Reading data with proc. rank %d and distributing data to other procs\n",rank);
        FILE *infile=fopen(input_filename,"r");
        for(int iproc=0; iproc<num_procs; iproc++){
            int local_n = get_local_n(global_n,iproc,num_procs);
            //printf("Data going to process %d is:\n",iproc);
            for(int iloc=0; iloc<local_n*m; iloc++){
                fscanf(infile, "%lf\n", (*local_x)+iloc);
                //printf("M %e\n",(*local_x)[iloc]);
            }
            if(iproc < num_procs-1){
                int tag=iproc;
                MPI_Send((*local_x),local_n*m,MPI_DOUBLE,iproc,tag,MPI_COMM_WORLD);
            }
        }
        fclose(infile);
    }else{
        int local_n = get_local_n(global_n,rank,num_procs);
        int tag=rank;
        MPI_Recv((*local_x),local_n*m,MPI_DOUBLE,num_procs-1,tag,MPI_COMM_WORLD,status);
    }
}

void mat_vec_parallel(double ** output, double * local_M, double * v, int n, int m, int rank, int num_procs){
    int local_n_max=get_local_n(n, 0, num_procs);
    int local_n=get_local_n(n, rank, num_procs);
    *output = (double *)malloc(local_n_max*sizeof(double));
    
    for(int row=0; row<local_n; row++){
        (*output)[row]=0.0;
        for(int col=0; col<m; col++){
            (*output)[row] += local_M[row*m+col]*v[col]; 
        }
    }
}

void write_vector_parallel(double * Mv, int global_n, int rank, int num_procs){
    MPI_Status * status;
    if(rank==0){
        int local_n=get_local_n(global_n,rank,num_procs);
        int j=0;
        for(int i=0; i<local_n; i++){
            printf("Mv[%d]= %.12f\n",j,Mv[i]);
            j++;
        }
        for(int iproc=1; iproc<num_procs; iproc++){
            local_n=get_local_n(global_n,iproc,num_procs);
            int tag=iproc;
            MPI_Recv(Mv,local_n,MPI_DOUBLE,iproc,tag,MPI_COMM_WORLD,status);
            for(int i=0; i<local_n; i++){
                printf("Mv[%d]= %.12f\n",j,Mv[i]);
                j++;
            }
        }
    }
    else{
        int local_n=get_local_n(global_n,rank,num_procs);
        int tag=rank;
        MPI_Send(Mv,local_n,MPI_DOUBLE,0,tag,MPI_COMM_WORLD);
    }
}

int main(int argc, char * argv[]){
    int num_procs,rank;
    MPI_Init     (&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(argc != 5){
        printf("usage: %s inputfile_matrix inputfile_vector n m\n",argv[0]);
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    
    int global_n=atoi(argv[3]);
    int m=atoi(argv[4]);
   
    double * local_M;
    double * v; 
    read_array_serial(&v,m,argv[2]);
    MPI_Barrier(MPI_COMM_WORLD);

    read_matrix_parallel(&local_M,global_n,m,rank,num_procs,argv[1]); 
    MPI_Barrier(MPI_COMM_WORLD);
    double * local_Mv;
    mat_vec_parallel(&local_Mv,local_M,v,global_n,m,rank,num_procs);

    MPI_Barrier(MPI_COMM_WORLD);
    
    write_vector_parallel(local_Mv,global_n,rank,num_procs);

    MPI_Finalize();
}
