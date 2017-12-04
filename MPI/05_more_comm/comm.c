#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char * argv[]){
    MPI_Init(&argc,&argv);
    
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if(rank==0){
        printf("-----------------------------------------\n");
        printf("Demo #1: circular shift with MPI_Sendrecv\n");
        printf("-----------------------------------------\n");}
    MPI_Barrier(MPI_COMM_WORLD);

    int x[2];
    x[0] = rank*2;
    x[1] = rank*2+1;
    printf("Before sendrecv, thread #%d x {%d, %d}\n", rank, x[0],x[1]); 
    MPI_Status * status;
    MPI_Sendrecv(x,2,MPI_INT,(rank+1)%num_procs,rank,x,2,MPI_INT,(rank-1)%num_procs,(rank-1)%num_procs,MPI_COMM_WORLD,status);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("After  sendrecv, thread #%d x {%d, %d}\n", rank, x[0],x[1]); 
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        printf("-----------------------------------------\n");
        printf("Demo #2: broadcast\n");
        printf("-----------------------------------------\n");}
    MPI_Barrier(MPI_COMM_WORLD);
    int y[2];
    y[0] = rank*2;
    y[1] = rank*2+1;
    printf("Before broadcast, thread #%d y {%d, %d}\n", rank, y[0],y[1]); 
    MPI_Bcast(y,2,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("After  broadcast, thread #%d y {%d, %d}\n", rank, y[0],y[1]); 
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        printf("-----------------------------------------\n");
        printf("Demo #3: gather\n");
        printf("-----------------------------------------\n");}
    MPI_Barrier(MPI_COMM_WORLD);
    int z[2];
    z[0] = rank*2;
    z[1] = rank*2+1;
    printf("Before gather, thread #%d z {%d, %d}\n", rank, z[0],z[1]); 
    MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Bcast(y,2,MPI_INT,0,MPI_COMM_WORLD);
    int * zz = (int *)malloc(2*num_procs*sizeof(int));
    MPI_Gather(z,2,MPI_INT,zz,2,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        printf("After  gather, thread 0 zz {");
        for(int i=0; i<2*num_procs; i++){
            printf("%d, ",zz[i]);
        } 
        printf("}\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        printf("-----------------------------------------\n");
        printf("Demo #4: scatter\n");
        printf("-----------------------------------------\n");}
    MPI_Barrier(MPI_COMM_WORLD);
    int * ww;
    if(rank==0){
        ww = (int *)malloc(2*num_procs*sizeof(int));
        for(int i=0; i<2*num_procs; i++){
            ww[i]=i;
        }
        printf("Before scatter, thread 0 ww {");
        for(int i=0; i<2*num_procs; i++){
            printf("%d, ",ww[i]);
        } 
        printf("}\n");
    }
    int w[2];
    MPI_Scatter(ww,2,MPI_INT,w,2,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("After scatter, thread #%d w {%d, %d}\n", rank, w[0],w[1]); 


    MPI_Finalize();
}
