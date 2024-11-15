test:
	g++ -c test.cpp
	g++ -g test.o -o testDebug
	g++ -O3 test.o -o testOptiimized