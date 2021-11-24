default: a

a.o: main.c 
    gcc -c main.c -o a.o

a: a.o
    gcc a.o -o a

clean:
    -rm -f a.o
    -rm -f a