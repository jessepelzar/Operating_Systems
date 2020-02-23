TARGETS = pelzar-j-CS311-p3

CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -pthread

all: clean $(TARGETS)

$(TARGETS):
	$(CC_C) $(CFLAGS) $@.c -o $@

clean:
	rm -f $(TARGETS)
