CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread -DFILTER_SIZE=10
LDFLAGS = -lboost_system -lboost_thread
TARGET = filter
SRC = main.cc
OBJ = $(SRC:.cc=.o)

filter: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean run

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET) | tee result/output.txt
