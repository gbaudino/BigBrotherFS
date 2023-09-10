CC := gcc

# To eliminate debugging messages use -DNDEBUG
CFLAGS := -O0 -std=gnu11 -Wall -Werror -Wno-unused-parameter -Werror=vla -g \
	  -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26 -D_GNU_SOURCE
CPPFLAGS := `pkg-config --cflags glib-2.0`
LDFLAGS :=`pkg-config --libs glib-2.0` -lfuse

export CC
export CFLAGS
export CPPFLAGS
export LDFLAGS


HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TARGET := fat-fuse

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


test:
	make -C tests test_ht
	sleep 3
	make -C tests test_ft
	sleep 3
	rm -rf ../resources
	mkdir ../resources
	cp ../original_imgs/* ../resources
	cp tests/test_fs.sh .
	sudo chmod +x test_fs.sh
	./test_fs.sh
	rm -f test_fs.sh
	rm -rf ../resources
	sleep 3
	clear
	
	
test-ht: hierarchy_tree.o
	make -C tests test_ht

test-ft:
	make -C tests test_ft

clean:
	rm -f $(TARGET) $(OBJECTS) tags cscope* *.img
	make -C tests clean
	rm -rf mnt
	clear

.PHONY: clean
