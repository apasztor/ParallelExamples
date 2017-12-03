for size in 201 4321 16000 30001; do
    echo "------------------------------ global size: $size -------------------------------------------"
    echo -e "import random\nfor i in range(${size}): print(random.uniform(0,1))" | python3 > input.dat
    
    cat input.dat | awk 'BEGIN{x=0;y=0;z=0;}\
                         {x+=$1;y+=$1*$1;z+=sin($1)}\
                         END{printf("Sum by awk       |                           |   ");\
                         printf("Sum        : %f   |   Square sum : %f   |   Sine sum   : %f\n", x,y,z)}'
    
    for nproc in 1 2 4 8; do
        mpirun -np $nproc ./sum input.dat ${size}
    done
done
