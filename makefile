CXX=g++#-10
LIBS=lib
BUILD=build
SRC=src
CFLAGS=-pthread -I $(LIBS)

#FIXME Do these all need to have BUILD in front? or can they be more listy and add that later?
# objects = $(BUILD)/main.o $(BUILD)/boatsAndBoards.o $(BUILD)/jsoncpp.o $(BUILD)/mcBoats.o
objects = $(BUILD)/boatsAndBoards.o $(BUILD)/jsoncpp.o $(BUILD)/mcBoats.o

.PHONY: clean
.PHONY: tests

$(BUILD)/runner.out:  $(objects) $(BUILD)/main.o
	$(CXX) $(objects) $(BUILD)/main.o -o $(BUILD)/runner.out $(CFLAGS)

$(BUILD)/unitTests.out: $(BUILD)/tests.o $(objects)
	$(CXX) $(BUILD)/tests.o $(objects) -o $(BUILD)/unitTests.out $(CFLAGS)

$(BUILD)/jsoncpp.o: $(LIBS)/jsoncpp.cpp $(LIBS)/json/json.h $(LIBS)/json/json-forwards.h
	$(CXX) -c $(LIBS)/jsoncpp.cpp -o $(BUILD)/jsoncpp.o

# compiles any file with extention .cpp
$(BUILD)/%.o: $(SRC)/%.cpp $(SRC)/%.h # should this direct to build? or is .o fine?
	$(CXX) -c $(SRC)/$*.cpp $(CFLAGS) -o $(BUILD)/$*.o

# special main.o as .h isn't needed
$(BUILD)/main.o: $(SRC)/main.cpp
	$(CXX) -c $(SRC)/main.cpp $(CFLAGS) -o $(BUILD)/main.o


clean:
	rm -f $(BUILD)/*
tests:
	make $(BUILD)/unitTests.out