prime : main.o ProcessPool.o
	g++ main.o ProcessPool.o -o prime

main.o :src/main.cpp
	g++ -c src/main.cpp -o  main.o -std=c++11

ProcessPool.o :src/ProcessPool.cpp
	g++ -c src/ProcessPool.cpp -o  ProcessPool.o -std=c++11

clean:
	rm *.o
