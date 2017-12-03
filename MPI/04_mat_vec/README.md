# MPI Matrix times vector based on dot product

Data storage:
 * All of the input vector is kept by all of the processes.
 * The matrix is divided into horizontal strips, each process 
   only keeps a strip of its own. 
Notice:
 * for reading in data, it is more natural to do it with the last
   process, since we can send the first chunks of data to the
   others, and just keep the last chunk for ourselves
 * for writing to output, it is more natural to use the first
   process, since we can just first write out what we already
   have, and later receive the other data
