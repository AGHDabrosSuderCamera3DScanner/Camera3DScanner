includes= -I ../../lib -I ../../modules/core -I ../../modules/core/computation -I ../../modules/core/image_analysis

flags= -Wall -O3 -flto -std=c++11 $(includes)

post_flags= -pthread

all: *.cpp *.hpp
	$(MAKE) -C computation/
	$(MAKE) -C image_analysis/
	g++ $(flags) -c -o core.o core.cpp $(post_flags)

clean:
	$(MAKE) -C computation/ clean
	$(MAKE) -C image_analysis/ clean
	rm -f *.o 
