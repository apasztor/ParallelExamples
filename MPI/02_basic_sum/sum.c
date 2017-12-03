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
    }else{
        int local_n = get_local_n(global_n,rank,num_procs);
        int tag=rank;
        MPI_Recv((*local_x),local_n,MPI_DOUBLE,num_procs-1,tag,MPI_COMM_WORLD,status);
    }
}

double serial_sum(double loc_x[], int loc_n){
    /* serial sum running on one process */
    double loc_sum=0.0;
    for(int i=0; i<loc_n; i++){
        loc_sum += loc_x[i];
    }
    return loc_sum;
}

double serial_funsum(double loc_x[], int loc_n, double (*f)(double)){
    /* serial sum running on one process - generic version using a function pointer */
    double loc_sum=0.0;
    for(int i=0; i<loc_n; i++)
        loc_sum += f(loc_x[i]);
    return loc_sum;
}

double parallel_sum(double local_x[], int global_n, int rank, int num_procs){
    /* parallel sum for the global array */
    int local_n = get_local_n(global_n, rank, num_procs);
    double local_sum = serial_sum(local_x, local_n);
    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, num_procs-1, MPI_COMM_WORLD);
    return global_sum;
}

double parallel_funsum(double local_x[], int global_n, int rank, int num_procs, double (*f)(double)){
    /* parallel sum for the global array - generic version with a function pointer */
    int local_n = get_local_n(global_n, rank, num_procs);
    double local_sum = serial_funsum(local_x, local_n, f);
    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, num_procs-1, MPI_COMM_WORLD);
    return global_sum;
}

double sq(double x){return x*x;}

int main(int argc, char * argv[]){
    int num_procs,rank;
    MPI_Init     (&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(argc != 3){
        printf("usage: %s inputfile global_n\n",argv[0]);
        MPI_Abort(MPI_COMM_WORLD,1);
    }
   
    int global_n=atoi(argv[2]);
    //if(rank == num_procs-1){
    //    printf("Local size in the processes:");
    //    for(int i=0; i<num_procs; i++){
    //        printf(" %d ",get_local_n(global_n,i,num_procs));
    //    }
    //    printf("\n");
    //}

    MPI_Barrier(MPI_COMM_WORLD);
    
    double * local_x;
    read_array(&local_x,global_n,rank,num_procs,argv[1]);

    double t1;
    t1 = MPI_Wtime(); 
    double sum  = parallel_sum   (local_x,global_n,rank,num_procs);
    double sum2 = parallel_funsum(local_x,global_n,rank,num_procs,&sq);
    double sum3 = parallel_funsum(local_x,global_n,rank,num_procs,&sin);
    
    if(rank==num_procs-1){
        printf("Num_procs  : %d   |   ",num_procs);
        printf("Calc_time  : %f   |   ",MPI_Wtime()-t1);
        printf("Sum        : %f   |   ",sum);
        printf("Square sum : %f   |   ",sum2);
        printf("Sine sum   : %f\n",sum3);
    }

    MPI_Finalize();
    free(local_x);    
}
