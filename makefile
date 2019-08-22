all: kiwi

SOURCES := $(wildcard *.c)
INCLUDE_DIRS := .

INCLUDE_FILES := $(foreach dir,$(INCLUDE_DIRS),$(wildcard $(dir)/*.h))

kiwi: $(SOURCES) $(INCLUDE_FILES)
	gcc $(addprefix -I,$(INCLUDE_DIRS)) $(SOURCES) -o $@

clean:
	rm -f kiwi

.PHONY: clean