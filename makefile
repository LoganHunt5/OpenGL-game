#CC specifies which compiler we're using
CC = g++
CFLAGS = -g3 -ggdb -Wall

#TARGET specifies the name of our exectuable
TARGET = test 

OBJ = shoot.o glad.o

LIB = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIB) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(TARGET)

run: $(TARGET)
	./$(TARGET)
