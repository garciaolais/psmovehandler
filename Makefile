TARGET = psmovehandler
cc = g++
CFLAGS = -rdynamic -lzmq -lpthread -lm -ludev -lbluetooth -lpsmoveapi
INCLUDES = include

default:all

all:
	$(cc) main.cpp -o $(TARGET) -I $(INCLUDES) $(CFLAGS)

clean:
	rm $(TARGET) 
