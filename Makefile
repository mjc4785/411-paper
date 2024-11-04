objects = Z80.cpp test_framework.c
all: $(objects)

$(objects): %: %.c
        $(CC) $(CFLAGS) -o $@ $<