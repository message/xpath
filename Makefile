CXXFLAGS =	-O2 -g -Wall -fmessage-length=0
CFLAGS = -std=c++11
OBJS =		xpath.o

LIBS = -I /usr/local/Cellar/boost/1.56.0/include/ -L /usr/local/Cellar/boost/1.56.0/lib/ -lboost_system -lboost_filesystem
TARGET =	xpath

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
