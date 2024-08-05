CXX=g++#-10
LIBS=lib
BUILD=build
SRC=src
CFLAGS=-pthread -I $(LIBS) #-O3 is more optimised

#FIXME Do these all need to have BUILD in front? or can they be more listy and add that later?
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
$(BUILD)/%.o: $(SRC)/%.cpp $(SRC)/%.h
	$(CXX) -c $(SRC)/$*.cpp $(CFLAGS) -o $(BUILD)/$*.o

# rule for the .cpp files that don't have coresponding .h files (ie main and tests)
$(BUILD)/%.o: $(SRC)/%.cpp
	$(CXX) -c $(SRC)/$*.cpp $(CFLAGS) -o $(BUILD)/$*.o

clean:
	rm -f $(BUILD)/*
tests:
	make $(BUILD)/unitTests.out