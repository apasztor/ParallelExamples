# Hello OpenMP

This is our first example program. It demonstrates several things in OpenMP:
 * Requesting a number of threads and checking how many threads we actually got
 * Using a specific thread for output
 * Using a barrier for synchronization, this makes sure we always get the 
   first message, i.e. how many threads we got, and only later the hellos
   from the threads
 * Run it several times, you will see that the hello messages change place.
   This is how it should be. The threads at that point are not synchronized.
