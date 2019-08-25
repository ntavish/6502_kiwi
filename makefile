all: kiwi

SOURCES := $(wildcard *.c)
INCLUDE_DIRS := .

INCLUDE_FILES := $(foreach dir,$(INCLUDE_DIRS),$(wildcard $(dir)/*.h))

kiwi: $(SOURCES) $(INCLUDE_FILES)
	gcc -Wall -Wno-unused-parameter -Wno-return-type $(addprefix -I,$(INCLUDE_DIRS)) $(SOURCES) -o $@

run: kiwi
	./kiwi

clean:
	rm -f kiwi

.PHONY: clean