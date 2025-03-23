CC = cc
LDFLAG = -s
CFLAGS = -O3
LDLIBS = -lX11 -lpng

SRC = xeshot.c
TARGET = xeshot

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
