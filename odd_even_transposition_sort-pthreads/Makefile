TARGET=psort
CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -ggdb3 -O3
LIBS := -lpthread
EXT := c
BUILDDIR := build

override BUILDDIR := $(strip $(BUILDDIR))
SOURCES := $(wildcard *.$(EXT))
OBJECTS := $(patsubst %.$(EXT), $(BUILDDIR)/%.o, $(SOURCES))
DEPS := $(patsubst %.$(EXT), $(BUILDDIR)/%.dep, $(SOURCES))

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS) $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

$(OBJECTS): $(BUILDDIR)/%.o: %.$(EXT) $(BUILDDIR)/%.dep $(BUILDDIR)/.tag
	$(CC) $(CFLAGS) -c $< -o $@

$(DEPS): $(BUILDDIR)/%.dep: %.$(EXT) $(BUILDDIR)/.tag
	mkdir -p $(dir $(@))
	$(CC) $(CFLAGS) -MM $< -MT $@ -MT $(<:.$(EXT)=.o) -o $@

%.tag:
	mkdir -p $(dir $(@))
	touch $@

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
	rm $(TARGET)

