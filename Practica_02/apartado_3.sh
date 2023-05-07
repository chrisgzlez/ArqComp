#!/bin/bash

# Solicitamos un nodo con 64 cores y 256 GB de memoria durante 2 horas
#SBATCH -n 1 -c 64 -t 02:00:00 --mem=256G
# Ponemos nombre a nuestro trabajo para poder identificarlo.
# ATENCIÓN - Debes sustituir el NN por el número de equipo.
#SBATCH --job-name p2acgNN

# Codes must print only f value, you can use debug mode to print anything else (#define DEBUG) 

# Number of cores (change to desired number)
C=4

# Flag to compare
flag=0

gcc p2_apartado3.c  -o p23 -O0 -march=native 

echo "N, ck" >> "results/results/version3_o0.csv"

for i in {1..10}
do
for N in 250 500 750 1000 1500 2000 2550 3000
do

    echo "Ejercicio 3"
    F5=`./p23 $N`
    echo "$N, $F5" >> "results/results/version3_o0.csv"
    echo $F5

    echo "##############################s####"
    echo ""

    # Check f is the same in all codes
    if [[ $F1 == $F2 ]] && [[ $F1 == $F3 ]] && [[ $F1 == $F4 ]] && [[ $F1 == $F5 ]] && [[ $F1 == $F6 ]] && [[ $F1 == $F7 ]]
    then
        echo "Everything is fine so far"
    else
        flag=1
    fi

done
done

if [[ $flag == 0 ]]
then
    echo "Everything is fine"
else
    echo "Wrong F. Check it out."
fi

