# Parallel sum and product with MPI

This is our second example program for MPI. It demonstrates:
 * using one process to read data and distribute it to the different processes
 * notice that we never allocate enough memory to fit all of the data, only the
   local data that is used by each process
 * using MPI_Reduce to calulate a sum in parallel
 * using function pointers to make our summation function a little more generic
 * a little check of code correctness with a shell script
 * We do not assume that the data size is divisible by the number of processes. 
   Here are a few examples of how long the strips are in each process:
    - Example #1: num_procs=2 n=8  -> local_n = 4,4
    - Example #2: num_procs=2 n=7  -> local_n = 4,3
    - Example #3: num_procs=4 n=16 -> local_n = 4,4,4,4
    - Example #4: num_procs=4 n=17 -> local_n = 5,5,5,2 
    - Example #5: num_procs=5 n=25 -> local_n = 5,5,5,5,5
    - Example #6: num_procs=5 n=28 -> local_n = 6,6,6,6,4 


