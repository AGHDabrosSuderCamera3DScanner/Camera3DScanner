flags= -O3 -flto -std=c++11 -I ../../lib -I ../../modules/communication -I ../../modules/core -I ../../modules/core/computation -I ../../modules/core/image_analysis -I ../../modules/display -I ../../modules/persistance

post_flags= -pthread

all: *.cpp *.hpp
	g++ $(flags) -c -o persistance.o *.cpp $(post_flags)

clean:
	rm -f *.o 
