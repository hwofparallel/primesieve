CC := g++
FLAGS := -std=c++11 -w

bin/primeSieve: build/ProcessPool.o build/main.o
			@mkdir -p bin
			$(CC) $(FLAGS) -I./include build/ProcessPool.o build/main.o -o $@
build/ProcessPool.o: src/ProcessPool.cpp
			@mkdir -p build
			$(CC) $(FLAGS) -I./include -c -o $@ src/ProcessPool.cpp
build/main.o: src/main.cpp
			@mkdir -p build
			$(CC) $(FLAGS) -I./include -c -o $@ src/main.cpp
clean:
	@rm -rf build
	@rm -rf bin