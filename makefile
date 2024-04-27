.PHONY: all clean

SRCS:=$(wildcard *.c)
BINS:=$(SRCS:%.c=%.bin)

CFLAGS+=-ggdb
LDLIBS+=-pthread

all: $(BINS)

%.bin: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o *.bin

