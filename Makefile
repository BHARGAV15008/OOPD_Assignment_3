test:
	g++ -c program1.cpp
	g++ -g program1.o -o program1Debug
	g++ -O3 program1.o -o program1Optimized

	g++ -c program2.cpp
	g++ -g program2.o -o program2Debug
	g++ -O3 program2.o -o program2Optimized