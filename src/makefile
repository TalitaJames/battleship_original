CXX=g++#-10
CFLAGS=-pthread -I ../lib/

.PHONY: clean

runner.out:  ../lib/jsoncpp.o mcTreesearch.o boatsAndBoards.o main.o
	$(CXX)  ../lib/jsoncpp.o mcTreesearch.o boatsAndBoards.o main.o -o runner.out $(CFLAGS)

unittests.out: tests.o jsoncpp.o mcTreesearch.o boatsAndBoards.o
	$(CXX) tests.o jsoncpp.o mcTreesearch.o boatsAndBoards.o -o unittests.out $(CFLAGS)

jsoncpp.o: ../lib/jsoncpp.cpp ../lib/json/json.h ../lib/json/json-forwards.h
	$(CXX) -c  ../lib/jsoncpp.cpp 

# compiles any file with extention .cpp
%.o: %.cpp
	$(CXX) -c $*.cpp $(CFLAGS)

clean:
	rm -f *.o *.out