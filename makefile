all: kiwi

SOURCES := $(wildcard *.c)
INCLUDE_DIRS := .

INCLUDE_FILES := $(foreach dir,$(INCLUDE_DIRS),$(wildcard $(dir)/*.h))

CFLAGS += -Wall -Wextra -Wno-unused-parameter
CFLAGS += -O0 -ggdb

kiwi: $(SOURCES) $(INCLUDE_FILES)
	gcc $(CFLAGS) $(addprefix -I,$(INCLUDE_DIRS)) $(SOURCES) -o $@

run: kiwi
	./kiwi

clean:
	rm -f kiwi

.PHONY: clean