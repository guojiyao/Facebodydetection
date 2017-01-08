CFLAGS = `pkg-config --cflags opencv`  
LIBS = `pkg-config --libs opencv`
HEADERS = -I. -I./include
LDFLAGS = -pthread -lpthread


bm: *.cpp  
	g++ $(CFLAGS) -o $@ *.cpp include/dlib/all/source.cpp  $(LIBS) $(HEADERS) -std=c++11 $(LDFLAGS)

clear:
	rm bm
