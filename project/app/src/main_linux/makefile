includes= -I ../lib -I ../modules/communication -I ../modules/core -I ../modules/core/computation -I ../modules/core/image_analysis -I ../modules/display -I ../modules/persistance

flags= -O2 -std=c++11 $(includes)

object_files= ../lib/common.o ../modules/communication/communication.o ../modules/core/core.o ../modules/core/computation/core_computation.o ../modules/core/image_analysis/core_image_analysis.o ../modules/display/display.o ../modules/persistance/persistance.o

post_flags= -pthread

all:
	g++ $(flags) -c -o main.o *.cpp $(post_flags)
	g++ $(flags) -o main main.o $(object_files) $(post_flags)

clean:
	rm -f *.o main
