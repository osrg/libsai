all:
	gcc -I ${SAIDIR}/inc rocker/*.c netdev/*.c libsai.c main.c

debug:
	gcc -I ${SAIDIR}/inc rocker/*.c netdev/*.c libsai.c main.c -g -O0
