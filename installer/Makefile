VERSION = 0.2.1
SOURCE = ../
PREFIX = $(MINGW_PREFIX)
DATA   = packages/de.darc.dm3mat.anytone_emu_gui/data/
DLLS   = libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll libfreetype-6.dll \
	 libharfbuzz-0.dll libmd4c.dll libpng16-16.dll zlib1.dll libdouble-conversion.dll \
   libicuin77.dll libicuuc77.dll libicudt77.dll libb2-1.dll libpcre2-16-0.dll libzstd.dll \
	 libbz2-1.dll libbrotlidec.dll libglib-2.0-0.dll libgraphite2.dll libintl-8.dll \
	 libiconv-2.dll libbrotlicommon.dll libpcre2-8-0.dll
ICONS  = anytone-emu-gui.ico

DEPS   = $(addprefix $(DATA), $(DLLS) $(ICONS))

.PHONY: installer deploy

all: installer

installer: anytone-emu-$(VERSION).exe

anytone-emu-$(VERSION).exe: $(DEPS) deploy 
	binarycreator -v --offline-only -c config/config.xml -p packages $@
	

deploy: $(DATA)/anytone-emu-gui.exe
	windeployqt6 $(<)

clean: 
	rm -r $(DATA) || true
	mkdir $(DATA)

%.dll: 
	install -D -t $(DATA) $(PREFIX)/bin/$(@F)

%.ico: 
	install -D -t $(DATA) $(SOURCE)/shared/icons/windows/$(@F)
