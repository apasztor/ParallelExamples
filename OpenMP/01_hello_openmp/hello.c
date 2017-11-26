#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAXTHREADS 4

int main(int argc, char * argv[]){
    if(argc!=2){
        printf("Correct program usage: %s num_of_threads_requested\n", argv[0]);
        exit(-1);
    }
    int nthd_req = atoi(argv[1]);
    if(nthd_req > MAXTHREADS){
        printf("You can request at most %d threads\n",MAXTHREADS);
        exit(-1);
    }

    omp_set_num_threads(nthd_req);

    #pragma omp parallel
    {
        int tid  = omp_get_thread_num();
        int nthd = omp_get_num_threads(); 
        if(tid==0){
            printf("Requested %d threads, got %d\n",nthd_req,nthd);
        }
        #pragma omp barrier

        printf("Hello from thread #%d\n",tid);
    }
 }
