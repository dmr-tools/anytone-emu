VERSION = 0.1.0
PREFIX = $(MINGW_PREFIX)
DATA   = packages/de.darc.dm3mat.anytone-emu-gui/data/
DLLS   = libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll libfreetype-6.dll \
	 libharfbuzz-0.dll libmd4c.dll libpng16-16.dll zlib1.dll libdouble-conversion.dll \
	 libicuin73.dll libicuuc73.dll libicudt73.dll libb2-1.dll libpcre2-16-0.dll libzstd.dll \
	 libbz2-1.dll libbrotlidec.dll libglib-2.0-0.dll libgraphite2.dll libintl-8.dll \
	 libiconv-2.dll libbrotlicommon.dll libpcre2-8-0.dll

DEPS   = $(addprefix $(DATA), $(DLLS))

.PHONY: installer deploy

all: installer
	
installer: anytone-emu-$(VERSION).exe

anytone-emu-$(VERSION).exe: $(DEPS) deploy 
	binarycreator -c config/config.xml -p packages $@
	

deploy: $(DATA)/anytone-emu-gui.exe
	windeployqt6 $(<)

clean: 
	rm -r $(DATA) || true
	mkdir $(DATA)

%.dll: 
	install -D -t $(DATA) $(PREFIX)/bin/$(@F)
