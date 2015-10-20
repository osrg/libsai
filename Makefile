all:
	gcc -I ${SAIDIR}/inc dataplane/rocker/*.c netdev/*.c impl/*.c libsai.c main.c -lpthread

debug:
	gcc -I ${SAIDIR}/inc dataplane/rocker/*.c netdev/*.c impl/*.c libsai.c main.c -g -O0 -lpthread
