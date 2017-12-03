for size in 2001 43201 160000 300001; do
    echo "------------------------------ global size: $size -------------------------------------------"
    echo -e "import random\nfor i in range(${size}): print(random.uniform(0,1))" | python3 > x.dat
    echo -e "import random\nfor i in range(${size}): print(random.uniform(0,1))" | python3 > y.dat
    
    paste x.dat y.dat | awk 'BEGIN{x=0}{x+=$1*$2}\
                             END{printf("Sum by awk       |                           |   ");\
                             printf("Dot prod   : %f\n", x,y,z)}'
  
    for nproc in 1 2 4 8; do
        mpirun -np $nproc ./dot x.dat y.dat ${size}
    done
done
