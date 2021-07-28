SRC  = $(wildcard *.md)
HTML = $(SRC:.md=.html)

.PHONY: all clean

all: $(HTML)

clean:
	$(RM) $(HTML)

WD := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# uses https://github.com/W4RH4WK/Dogx
include $(HOME)/git/dogx/template/settings.mk
