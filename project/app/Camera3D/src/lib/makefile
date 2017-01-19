flags= -Wall -O3 -flto -std=c++11

post_flags= -pthread

all:
	g++ $(flags) -c -o common.o common.cpp $(post_flags)
	g++ $(flags) -c -o xml_reader.o xml_reader.cpp $(post_flags)

clean:
	rm -f *.o
