#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define STRINGSIZE 256

int main(int argc, char * argv[]){
    MPI_Init(&argc,&argv);
    
    int my_rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    char message[STRINGSIZE];
    int tag=0;
    MPI_Status status;

    if(my_rank!=0){
       int dest=0;
       sprintf(message, "This is a message passed from proc. #%d to proc #%d\n",my_rank,dest);
       MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    }else{
       for(int src=1; src<num_procs; src++){
           MPI_Recv(message,STRINGSIZE,MPI_CHAR,src,tag,MPI_COMM_WORLD,&status);
           printf("%s\n",message);
       }
    }

    MPI_Finalize();
}
