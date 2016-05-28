flags= -std=c++11 -I ../lib
post_flags= -pthread

all: *.cpp
	g++ $(flags) -o main *.cpp $(post_flags)

clean:
	rm -f *.o main
