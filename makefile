d1:	src/statusInit.cxx src/summaryPlots.cxx src/swizzlePlots.cxx
	g++ -g -std=c++11 -c src/statusInit.cxx -o statusInit.o $(shell root-config --cflags) -I./include
	g++ -g -std=c++11 -c src/summaryPlots.cxx -o summaryPlots.o $(shell root-config --cflags) -I./include
	g++ -g -std=c++11 -c src/swizzlePlots.cxx -o swizzlePlots.o $(shell root-config --cflags) -I./include
	g++ -g -std=c++11 -c src/eventTest.cxx -o eventTest.o $(shell root-config --cflags) -I./include
	g++ -g -std=c++11  -Wall -o statusInit statusInit.o `root-config --cflags --glibs`
	g++ -g -std=c++11  -Wall -o summaryPlots summaryPlots.o `root-config --cflags --glibs`
	g++ -g -std=c++11  -Wall -o swizzlePlots swizzlePlots.o `root-config --cflags --glibs`
	g++ -g -std=c++11  -Wall -o eventTest eventTest.o `root-config --cflags --glibs`
	rm *.o
