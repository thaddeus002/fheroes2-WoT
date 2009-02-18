# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
# DEVELOPMENT: build with development extensions
#
# WITHOUT_UNICODE: build without unicode (disable translation and ttf font)
# WITHOUT_PNG: build without png support (disable cache image, icn2png)
# WITHOUT_XML: build without xml support (disable save game)

TARGET := fheroes2

all:
	$(MAKE) -C src
	@cp src/dist/$(TARGET) .
	@cp src/dist/$(TARGET).pot files/lang

clean:
	$(MAKE) -C src clean
