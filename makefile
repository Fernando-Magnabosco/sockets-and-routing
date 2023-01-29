make: 
	clear
	gcc main.c \
	src/*.c \
	-o router -lpthread