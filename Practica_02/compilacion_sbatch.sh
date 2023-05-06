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

gcc p2_apartado1.c  -o p21o0 -O0
gcc p2_apartado1.c  -o p21o2 -O2
gcc p2_apartado1.c  -o p21o3 -O3

gcc p2_apartado2.c  -o p22 -O0

gcc p2_apartado3.c  -o p23 -O0 -march=native 

gcc p2_apartado4.c  -o p24o0 -O0 -fopenmp
gcc p2_apartado4_dynamic.c  -o p24o0_dynamic -O0 -fopenmp
gcc p2_apartado4_guided.c  -o p24o0_guided -O0 -fopenmp
gcc p2_apartado4_collapse.c  -o p24o0_collapse -O0 -fopenmp
gcc p2_apartado4.c  -o p24o2 -O2 -fopenmp

echo "N, ck" >> "results/results/version1_o0.csv"
echo "N, ck" >> "results/results/version1_o2.csv"
echo "N, ck" >> "results/results/version1_o3.csv"
echo "N, ck" >> "results/results/version2_o0.csv"
echo "N, ck" >> "results/results/version3_o0.csv"
echo "N, ck" >> "results/results/version4_o0.csv"
echo "N, ck" >> "results/results/version4_o0_dynamic.csv"
echo "N, ck" >> "results/results/version4_o0_guided.csv"
echo "N, ck" >> "results/results/version4_o0_collapse.csv"
echo "N, ck" >> "results/results/version4_o2.csv"

for i in 0 1 2 3 4 5 6 7 8 9
do
    for N in 250 500 750 1000 1500 2000 2550 3000
    do

        echo "##################################"
        echo "N: $N"
        echo "##################################"

        echo "Ejercicio 1 o0"
        F1=`./p21o0 $N`
        echo "$N, $F1" >> "results/results/version1_o0.csv"
        echo $F1

        echo "Ejercicio 1 o2"
        F2=`./p21o2 $N`
        echo "$N, $F2" >> "results/results/version1_o2.csv"
        echo $F2

        echo "Ejercicio 1 o3"
        F3=`./p21o3 $N`
        echo "$N, $F3" >> "results/results/version1_o3.csv"
        echo $F3

        echo "Ejercicio 2"
        F4=`./p22 $N`
        echo "$N, $F4" >> "results/results/version2_o0.csv"
        echo $F4

        echo "Ejercicio 3"
        F5=`./p23 $N`
        echo "$N, $F5" >> "results/results/version3_o0.csv"
        echo $F5

        echo "Ejercicio 4 o0"
        F6=`./p24o0 $N $C`
        echo "$N, $F6" >> "results/results/version4_o0.csv"
        echo $F6

        for C1 in 4 8 16 32 64
        do
            echo "##################################"
            echo "C: $C1"
            echo "##################################"

            F6=`./p24o0 $N $C1`
            echo "$N, $F6" >> "results/hilos/v4_h$C1.csv"
            echo $F6
        done

        echo "Ejercicio 4 o0 Dynamic"
        F7=`./p24o0_dynamic $N $C`
        echo "$N, $F7" >> "results/results/version4_o0_dynamic.csv"
        echo $F7

        echo "Ejercicio 4 o0 Guided"
        F8=`./p24o0_guided $N $C`
        echo "$N, $F8" >> "results/results/version4_o0_guided.csv"
        echo $F8

        echo "Ejercicio 4 o0 Collapse"
        F9=`./p24o0_collapse $N $C`
        echo "$N, $F9" >> "results/results/version4_o0_collapse.csv"
        echo $F9


        echo "Ejercicio 4 o2"
        F10=`./p24o2 $N $C`
        echo "$N, $F10" >> "results/results/version4_o2.csv"
        echo $F10

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

