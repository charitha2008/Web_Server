CC = gcc

WEBDIR = webserver
INCDIR = include
SRCDIR = src
OBJDIR = objects

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

CFLAGS = -I$(INCDIR) -Wall -pthread

all: $(WEBDIR)

$(WEBDIR): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(WEBDIR)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean: 
	rm -f -v $(OBJS)/*.o $(WEBDIR)

