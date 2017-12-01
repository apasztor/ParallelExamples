size=4000000

echo "-----------------------------------------"
echo "Generating random sequence               "
echo "-----------------------------------------"
echo -e "import random\nfor i in range(${size}): print(random.uniform(0,1))" | python3 > input.dat

echo "-----------------------------------------"
echo "Calculating sums with awk:"
echo "-----------------------------------------"

cat input.dat | awk 'BEGIN{x=0}{x+=$1}     END{printf("Sum        : %f\n", x)}'
cat input.dat | awk 'BEGIN{x=0}{x+=$1*$1}  END{printf("Square sum : %f\n", x)}'
cat input.dat | awk 'BEGIN{x=0}{x+=sin($1)}END{printf("Sine sum   : %f\n", x)}'

for nproc in 1 2 4 8; do
    echo "-----------------------------------------"
    echo "Sum with our MPI code on $nproc process"
    echo "-----------------------------------------"
    mpirun -np $nproc ./sum input.dat ${size}
done
