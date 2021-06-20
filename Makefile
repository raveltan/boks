all:
	gcc -Wall -Werror -lcap -lseccomp main.c -o boks
clean:
	rm ./boks

