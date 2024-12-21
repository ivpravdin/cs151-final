CXX = g++
CXXFLAGS = -Wall -std=c++17 -fopenmp -g -O3

TARGET = main
SRCS = main.cpp parallel_ullman.cpp
HEADERS = utils.hpp parallel_ullman.hpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean