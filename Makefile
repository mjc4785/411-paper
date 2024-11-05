objects = Z80.cpp
all: $(objects)

$(objects): %: %.c
        $(CC) $(CFLAGS) -o $@ $<