CXX=g++ #-10
CFLAGS=-pthread -g -O0

.PHONY: clean

runner.out: jsoncpp.o mcTreesearch.o boatsAndBoards.o main.o
	$(CXX) jsoncpp.o mcTreesearch.o boatsAndBoards.o main.o -o runner.out $(CFLAGS)

main.o: main.cpp
	$(CXX) -c main.cpp $(CFLAGS)

jsoncpp.o: jsoncpp.cpp json/json.h json/json-forwards.h
	$(CXX) -c jsoncpp.cpp $(CFLAGS)

mcTreesearch.o: mcTreesearch.cpp mcTreesearch.h
	$(CXX) -c mcTreesearch.cpp $(CFLAGS)

boatsAndBoards.o: boatsAndBoards.cpp boatsAndBoards.h
	$(CXX) -c boatsAndBoards.cpp $(CFLAGS)

clean:
	rm -f *.o *.out