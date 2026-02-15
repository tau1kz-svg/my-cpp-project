CXX = g++
CXXFLAGS = -O2 -Wall -std=c++20
LDFLAGS = -mwindows

TARGET = vs_player.exe
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)