CXX=g++#-10
LIBS=lib
BUILD=build
SRC=src
CFLAGS=-pthread -I $(LIBS)

objects = $(BUILD)/main.o $(BUILD)/boatsAndBoards.o $(BUILD)/jsoncpp.o $(BUILD)/mcBoats.o # $(BUILD)/mcTreesearch.o
# Do these all need to have BUILD in front? or can they be more listy and add that later?

.PHONY: clean

$(BUILD)/runner.out:  $(objects) # 
	$(CXX) $(objects) -o $(BUILD)/runner.out $(CFLAGS)

$(BUILD)/unittests.out: $(BUILD)/tests.o $(objects)
	$(CXX) $(BUILD)/tests.o $(objects) -o $(BUILD)/unittests.out $(CFLAGS)

$(BUILD)/jsoncpp.o: $(LIBS)/jsoncpp.cpp $(LIBS)/json/json.h $(LIBS)/json/json-forwards.h
	$(CXX) -c $(LIBS)/jsoncpp.cpp -o $(BUILD)/jsoncpp.o

# compiles any file with extention .cpp
$(BUILD)/%.o: $(SRC)/%.cpp # should this direct to build? or is .o fine?
	$(CXX) -c $(SRC)/$*.cpp $(CFLAGS) -o $(BUILD)/$*.o

clean:
	rm -f $(BUILD)/*