#!/bin/bash

# Solicitamos un nodo con 64 cores y 256 GB de memoria durante 2 horas
#SBATCH -n 1 -c 64 -t 02:00:00 --mem=256G
# Ponemos nombre a nuestro trabajo para poder identificarlo.
# ATENCIÓN - Debes sustituir el NN por el número de equipo.
#SBATCH --job-name p2acgNN

# Codes must print only f value, you can use debug mode to print anything else (#define DEBUG)

# Flag to compare
flag=0

gcc p2_apartado4.c  -o p24o0 -O0 -fopenmp

for C in 4 8 16 32 64
do
  echo "N, ck" >> "results/hilos/v4_h$C.csv"
done

for N in 250 500 750 1000 1500 2000 2550 3000
do

    echo "##################################"
    echo "N: $N"
    echo "##################################"

    for C in 4 8 16 32 64
    do
      echo "##################################"
      echo "C: $C"
      echo "##################################"

      F6=`./p24o0 $N $C`
      echo "$N, $F6" >> "results/hilos/v4_h$C.csv"
      echo $F6

    done
    echo "##################################"
    echo ""

    # Check f is the same in all codes
    if [[ $F1 == $F2 ]] && [[ $F1 == $F3 ]] && [[ $F1 == $F4 ]] && [[ $F1 == $F5 ]] && [[ $F1 == $F6 ]] && [[ $F1 == $F7 ]]
    then
        echo "Everything is fine so far"
    else
        flag=1
    fi

done

if [[ $flag == 0 ]]
then
    echo "Everything is fine"
else
    echo "Wrong F. Check it out."
fi

