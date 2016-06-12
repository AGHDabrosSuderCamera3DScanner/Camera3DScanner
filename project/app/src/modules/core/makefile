flags= -std=c++11 -I ../../lib -I ../../modules/communication -I ../../modules/core -I ../../modules/core/computation -I ../../modules/core/image_analysis -I ../../modules/display -I ../../modules/persistance

post_flags= -pthread

all: *.cpp *.hpp
	$(MAKE) -C computation/
	$(MAKE) -C image_analysis/
	g++ $(flags) -c -o core.o *.cpp $(post_flags)

clean:
	$(MAKE) -C computation/ clean
	$(MAKE) -C image_analysis/ clean
	rm -f *.o 
