CC = gcc
LD = gcc
INC_DIR = include
SRC_DIR = src
OBJ_DIR = objects
CFLAGS = -g -Wall -I$(INC_DIR)
LDFLAGS = 
SRCS = $(SRC_DIR)/server.c $(SRC_DIR)/network.c $(SRC_DIR)/process.c
OBJS = $(OBJ_DIR)/server.o $(OBJ_DIR)/network.o $(OBJ_DIR)/process.o
PROG = server
RM = /bin/rm

# all: greetings

all: $(PROG)

# greetings: main.o greeter.o
# 	gcc -o greetings main.o greeter.o

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

#main.o: main.c
#	gcc -g -c main.c

#greeter.o: greeter.c
#	gcc -g -c greeter.c

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#clean:
#	rm main.o greeter.o

clean:
	$(RM) $(PROG) $(OBJS)

