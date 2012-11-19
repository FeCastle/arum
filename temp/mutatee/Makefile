all : dinner_party.o stack.o verify.o
	gcc -o dinner_party dinner_party.o stack.o
	gcc -o verify verify.o

dinner_party.o : dinner_party.c
	gcc -c dinner_party.c

stack.o : stack.c
	gcc -c stack.c

verify.o : verify.c
	gcc -c verify.c

clean : 
	rm dinner_party
	rm *.o
	rm verify
