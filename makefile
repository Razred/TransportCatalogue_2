CC = g++
CXXFLAGS = -std=c++20

TARGET = myprogram
SRCS = main.cpp request_handler.cpp transport_catalogue.cpp json.cpp json_reader.cpp map_render.cpp svg.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)