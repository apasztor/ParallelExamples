n=79
m=83

python3 ./gen.py $n $m
mpirun -np 4 ./matvec M.txt v.txt $n $m > Mv_MPI.txt
paste Mv_MPI.txt Mv_numpy.txt | awk 'BEGIN{x=0;}{x+=($2-$3)*($2-$3);}END{print "Square difference between awk and MPI code: ", x;}'
