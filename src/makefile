CXX=g++-10

.PHONY: clean

runner.out: jsoncpp.o mcTreesearch.o boatsAndBoards.o main.o
	$(CXX) jsoncpp.o mcTreesearch.o boatsAndBoards.o main.o -o runner.out -pthread

main.o: main.cpp
	$(CXX) -c main.cpp -pthread

jsoncpp.o: jsoncpp.cpp json/json.h json/json-forwards.h
	$(CXX) -c jsoncpp.cpp

mcTreesearch.o: mcTreesearch.cpp mcTreesearch.h
	$(CXX) -c mcTreesearch.cpp

boatsAndBoards.o: boatsAndBoards.cpp boatsAndBoards.h
	$(CXX) -c boatsAndBoards.cpp -pthread

clean:
	rm -f *.o *.out