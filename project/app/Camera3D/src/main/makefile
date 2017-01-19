includes= -I ../lib -I ../modules/core -I ../modules/core/computation -I ../modules/core/image_analysis

flags= -Wall -O3 -flto -std=c++11 $(includes)

object_files= ../lib/common.o ../lib/xml_reader.o ../modules/core/core.o ../modules/core/computation/core_computation.o ../modules/core/image_analysis/core_image_analysis.o

post_flags= -pthread

all:
	g++ $(flags) -c -o main.o *.cpp $(post_flags)
	g++ $(flags) -o main main.o $(object_files) $(post_flags)

clean:
	rm -f *.o main
