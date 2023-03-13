#!/bin/bash
# Solicitamos un nodo con 64 cores y 256 GB de memoria durante 2 horas
#SBATCH -n 1 -c 64 -t 02:00:00 --mem=256G
# Ponemos nombre a nuestro trabajo para poder identificarlo.
# ATENCIÓN - Debes sustituir el NN por el número de equipo.
#SBATCH --job-name p1acgNN

# Sustituir los valores de Di y Li por los calculados para la realización de la práctica.
# 
# lscpu para obtener el tamaño de la cache y las lineas de esta
# S1 datos = 48KiB / 3MiB total cores con linea de 64 bytes
# S1 instruction = 32 KiB / 2 MiB total cores con linea de 64 bytes
# S2 = 1280 KiB / 80 MiB total cores con linea de 64 bytes
#
# R = Numero de elementos del array A (iteraciones) para leer L lineas cache
# R = (Numero de Doubles por Linea / D)*L = ( (TamLinea / TamDouble) / D ) * L = ( 8 / D ) * L
#  Si D >= 8; R = L (en cada iteracion leemos una liena nueva)
# D = Separacion de elementos para observar el principio de localidad
# L = numero de lineas cache diferentes. eg: L = 3; 3 lineas cache
#
#
# Un elemento do Vector A ten que ser de tamaño L*64 (lineas cache * tamaño linea cache)
# 
#
#
# Compilacion
# gcc acp1.c -o acp1 -msse3 -O0

S1=$(( (48 << 10) / 64 )) 	# Numero de lineas cache Nivel 1 
S2=$(( (1280 << 10) / 64 )) # Numero de lineas cache Nivel 2

L1=$(( $S1 / 2 )) 			# *0.5 Mitad de la cache nivel 1 
L2=$(( $S1 + ($S1 / 2) )) 	# *1.5 Toda la cache N1 y más
L3=$(( $S2 / 2 )) 			# *0.5 Mitad de la cache N2
L4=$(( ($S2 / 4) * 3 )) 	# *0.75 3 cuartos de la cache N2
L5=$(( $S2 * 2 )) 			# ...
L6=$(( $S2 * 4 ))
L7=$(( $S2 * 8 ))

array_d=(2 4 8 16 32 64 128 256 512 1024)

echo $S1
echo $S2


resultados=()

for i in {1..10} # numero de pruebas
do
	for D in ${array_d[@]} # potencias de 2. que permitan estudiar la localidad
	do
		for L in {$L1,$L2,$L3,$L4,$L5,$L6,$L7}
		do
			echo -n " "
			# ./acp1 $D $L
			#resultados+=($(./acp1 $D $L))
		done
	done
done

gcc -Wall acp1.c -o acp1

