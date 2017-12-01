echo "-----------------------------------------"
echo "Calculating sums with awk:"
echo "-----------------------------------------"

cat input.dat | awk 'BEGIN{x=0}{x+=$1}     END{printf("Sum        : %f\n", x)}'
cat input.dat | awk 'BEGIN{x=0}{x+=$1*$1}  END{printf("Square sum : %f\n", x)}'
cat input.dat | awk 'BEGIN{x=0}{x+=sin($1)}END{printf("Sine sum   : %f\n", x)}'

echo "-----------------------------------------"
echo "Calculating sums with our C code:"
echo "-----------------------------------------"

mpirun -np 8 ./sum
