CXX = g++
CXXFLAGS = -I. -Wall
LDFLAGS = -L. -llhsp01

TARGET = main
SRCS = main.cpp
HDRS = lhsp01.h

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) lhsp01.lib

clean:
	rm -f $(TARGET) *.o
