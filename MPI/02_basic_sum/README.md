# Parallel sum and product with MPI

This is our second example program for MPI. It demonstrates:
 * using one process to read data and distribute it to the different processes
 * notice that we never allocate enough memory to fit all of the data, only the
   local data that is used by each process
 * using MPI_Reduce to calulate a sum in parallel
 * using function pointers to make our summation function a little more generic
 * a little check of code correctness with a shell script
