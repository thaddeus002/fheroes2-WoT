# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
# DEVELOPMENT: build with development extensions
#
# WITHOUT_NETWORK: build without network support
# WITHOUT_UNICODE: build without unicode (disable translation and ttf font)
# WITHOUT_PNG: build without png support (disable cache image, icn2png)

TARGET := fheroes2

all:
	$(MAKE) -C src
	@cp src/dist/$(TARGET) .
ifndef WITHOUT_UNICODE
	@cp src/dist/$(TARGET).pot files/lang
endif

clean:
	$(MAKE) -C src clean
