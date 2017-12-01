size=1000000

echo "-----------------------------------------"
echo "Generating random sequence               "
echo "-----------------------------------------"
echo -e "import random\nfor i in range(${size}): print(random.uniform(0,1))" | python3 > x.dat
echo -e "import random\nfor i in range(${size}): print(random.uniform(0,1))" | python3 > y.dat

echo "-----------------------------------------"
echo "Calculating dot prod with awk:"
echo "-----------------------------------------"

paste x.dat y.dat | awk 'BEGIN{x=0}{x+=$1*$2}END{printf("Sum        : %f\n", x)}'

for nproc in 1 2 4 8; do
    echo "-----------------------------------------"
    echo "Sum with our MPI code on $nproc process"
    echo "-----------------------------------------"
    mpirun -np $nproc ./dot x.dat y.dat ${size}
done
