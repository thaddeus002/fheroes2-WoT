# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
# DEVELOPMENT: build with development extensions
#

TARGET := fheroes2

all:
	$(MAKE) -C src
	cp src/dist/$(TARGET) .

clean:
	$(MAKE) -C src clean
