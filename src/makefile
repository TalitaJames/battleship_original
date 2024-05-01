CXX=g++#-10
CFLAGS=-pthread

.PHONY: clean

runner.out: jsoncpp.o mcTreesearch.o boatsAndBoards.o main.o
	$(CXX) jsoncpp.o mcTreesearch.o boatsAndBoards.o main.o -o runner.out $(CFLAGS)

unittests.out: tests.o jsoncpp.o mcTreesearch.o boatsAndBoards.o
	$(CXX) tests.o jsoncpp.o mcTreesearch.o boatsAndBoards.o -o unittests.out $(CFLAGS)

tests.o: tests.cpp acutest.h
	$(CXX) -c tests.cpp $(CFLAGS)

main.o: main.cpp
	$(CXX) -c main.cpp $(CFLAGS)

jsoncpp.o: jsoncpp.cpp json/json.h json/json-forwards.h
	$(CXX) -c jsoncpp.cpp

mcTreesearch.o: mcTreesearch.cpp mcTreesearch.h
	$(CXX) -c mcTreesearch.cpp

boatsAndBoards.o: boatsAndBoards.cpp boatsAndBoards.h
	$(CXX) -c boatsAndBoards.cpp $(CFLAGS)

clean:
	rm -f *.o *.out