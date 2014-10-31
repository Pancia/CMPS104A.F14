GPP	= g++ -g -O0 -Wall -Wextra -std=gnu++11
GRIND	= valgrind --leak-check=full --show-reachable=yes

all	: oc

oc	: main.o auxlib.o stringset.o
	${GPP} main.o auxlib.o stringset.o -o oc

main.o	: main.cpp auxlib.h stringset.h
	${GPP} -c  main.cpp

auxlib.o	: auxlib.cpp auxlib.h
	${GPP} -c auxlib.cpp

stringset.o	: stringset.cpp stringset.h
	${GPP} -c stringset.cpp

clean	:
	-rm auxlib.o main.o stringset.o

test	: oc
	${GRIND} teststring * * * >test.out 2>test.err

