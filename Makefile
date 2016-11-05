CC = gcc

TARGET = myshell
SRCS = main.c
OBJS = main.o 

LDFLAGS = 
CFLAGS = -g -O
DFLAGS = -std=c99

RM = rm -f

all: $(TARGET)


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) -o $@ $^

.c.o:
	$(CC) -c $(DFLAGS) $<

clean:
	$(RM) $(OBJS)

clean_target:
	$(RM) $(TARGET)

clean_all:
	$(RM) $(TARGET) $(OBJS)
