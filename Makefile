include $(DEVKITARM)/base_tools

COMPARE ?= 0

CPP := $(CC) -E
LD := $(DEVKITARM)/bin/arm-none-eabi-ld

GAME_VERSION ?= USA
REVISION := 0
GAME_LANGUAGE := ENGLISH

TITLE       := GBAZELDA MC
MAKER_CODE  := 01

ifeq ($(GAME_VERSION), USA)
GAME_CODE   := BZME
BUILD_NAME  := tmc
else
ifeq ($(GAME_VERSION), DEMO_USA)
GAME_CODE   := BZHE
BUILD_NAME  := tmc_demo_usa
else
ifeq ($(GAME_VERSION), JP)
GAME_CODE   := BZMJ
BUILD_NAME  := tmc_jp
GAME_LANGUAGE := JAPANESE
else
ifeq ($(GAME_VERSION), DEMO_JP)
GAME_CODE   := BZMJ
BUILD_NAME  := tmc_demo_jp
GAME_LANGUAGE := JAPANESE
else
ifeq ($(GAME_VERSION), EU)
GAME_CODE   := BZMP
BUILD_NAME  := tmc_eu
else
$(error unknown version $(GAME_VERSION))
endif
endif
endif
endif
endif

SHELL := /bin/bash -o pipefail


ROM := $(BUILD_NAME).gba
OBJ_DIR := build/$(BUILD_NAME)

ELF = $(ROM:.gba=.elf)
MAP = $(ROM:.gba=.map)

C_SUBDIR = src
DATA_C_SUBDIR = src/data
ASM_SUBDIR = asm
DATA_ASM_SUBDIR = data
SONG_SUBDIR = sound/songs
MID_SUBDIR = sound/songs/midi
ASSET_SUBDIR = assets
ENUM_INCLUDE_SUBDIR = enum_include

C_BUILDDIR = $(OBJ_DIR)/$(C_SUBDIR)
ASM_BUILDDIR = $(OBJ_DIR)/$(ASM_SUBDIR)
ASM_ENUM_INCLUDE_DIR = $(ASM_BUILDDIR)/$(ENUM_INCLUDE_SUBDIR)
DATA_ASM_BUILDDIR = $(OBJ_DIR)/$(DATA_ASM_SUBDIR)
SONG_BUILDDIR = $(OBJ_DIR)/$(SONG_SUBDIR)
MID_BUILDDIR = $(OBJ_DIR)/$(MID_SUBDIR)
ASSET_BUILDDIR = $(OBJ_DIR)/$(ASSET_SUBDIR)
PREPROC_INC_PATHS = $(ASSET_BUILDDIR) $(ASM_ENUM_INCLUDE_DIR)

ASFLAGS := -mcpu=arm7tdmi --defsym $(GAME_VERSION)=1 --defsym REVISION=$(REVISION) --defsym $(GAME_LANGUAGE)=1 -I $(ASSET_SUBDIR) -I $(ASSET_BUILDDIR) -I $(ASM_ENUM_INCLUDE_DIR)

CC1             := tools/agbcc/bin/agbcc
override CFLAGS += -O2 -Wimplicit -Wparentheses -Werror -Wno-multichar -g3
# -fhex-asm

# ifeq ($(DINFO),1)
# override CFLAGS += -g
# endif

CPPFLAGS := -I tools/agbcc -I tools/agbcc/include -iquote include -nostdinc -undef -D$(GAME_VERSION) -DREVISION=$(REVISION) -D$(GAME_LANGUAGE)

LDFLAGS = -Map ../../$(MAP)

LIB := -L ../../tools/agbcc/lib -lc

SHA1 := $(shell { command -v sha1sum || command -v shasum; } 2>/dev/null) -c
GFX := tools/bin/gbagfx
AIF := tools/bin/aif2pcm
MID := tools/bin/mid2agb
SCANINC := tools/bin/scaninc
# TODO: use charmap?
PREPROC := tools/bin/preproc
FIX := tools/bin/gbafix
ASSET_PROCESSOR := tools/bin/asset_processor
ENUM_PROCESSOR := tools/extract_include_enum.py

ASSET_CONFIGS = assets/assets.json assets/gfx.json assets/map.json assets/samples.json assets/sounds.json
TRANSLATIONS = translations/USA.bin translations/English.bin translations/French.bin translations/German.bin translations/Spanish.bin translations/Italian.bin

# Clear the default suffixes
.SUFFIXES:
# Don't delete intermediate files
.SECONDARY:
# Delete files that weren't built properly
.DELETE_ON_ERROR:
# Secondary expansion is required for dependency variables in object rules.
.SECONDEXPANSION:


$(shell mkdir -p $(C_BUILDDIR) $(ASM_BUILDDIR) $(DATA_ASM_BUILDDIR) $(SONG_BUILDDIR) $(MID_BUILDDIR))

infoshell = $(foreach line, $(shell $1 | sed "s/ /__SPACE__/g"), $(info $(subst __SPACE__, ,$(line))))

# Build tools when building the rom
# Disable dependency scanning for clean/tidy/tools
ifeq (,$(filter-out all compare target,$(MAKECMDGOALS)))
$(call infoshell, $(MAKE) tools)
else
NODEP := 1
endif

interwork := $(C_BUILDDIR)/interrupts.o \
$(C_BUILDDIR)/collision.o \
$(C_BUILDDIR)/playerItem.o \
$(C_BUILDDIR)/object.o \
$(C_BUILDDIR)/manager.o \
$(C_BUILDDIR)/npc.o

$(interwork): CFLAGS += -mthumb-interwork
$(C_BUILDDIR)/gba/m4a.o: CFLAGS = -O2 -mthumb-interwork -Wimplicit -Wparentheses -Werror -Wno-multichar
$(C_BUILDDIR)/eeprom.o: CFLAGS = -O1 -mthumb-interwork -Wimplicit -Wparentheses -Werror -Wno-multichar

C_SRCS := $(wildcard $(C_SUBDIR)/*.c $(C_SUBDIR)/*/*.c)
C_OBJS := $(patsubst $(C_SUBDIR)/%.c,$(C_BUILDDIR)/%.o,$(C_SRCS))

ASM_SRCS := $(wildcard $(ASM_SUBDIR)/*.s $(ASM_SUBDIR)/*/*.s)
ASM_OBJS := $(patsubst $(ASM_SUBDIR)/%.s,$(ASM_BUILDDIR)/%.o,$(ASM_SRCS)) $(patsubst $(ASM_SUBDIR)/*/%.s,$(ASM_BUILDDIR)/**/%.o,$(ASM_SRCS))

DATA_ASM_SRCS := $(wildcard $(DATA_ASM_SUBDIR)/*.s $(DATA_ASM_SUBDIR)/**/*.s $(DATA_ASM_SUBDIR)/**/**/*.s)
DATA_ASM_OBJS := $(patsubst $(DATA_ASM_SUBDIR)/%.s,$(DATA_ASM_BUILDDIR)/%.o,$(DATA_ASM_SRCS))

SONG_SRCS := $(wildcard $(SONG_SUBDIR)/*.s)
SONG_OBJS := $(patsubst $(SONG_SUBDIR)/%.s,$(SONG_BUILDDIR)/%.o,$(SONG_SRCS))

MID_SRCS := $(wildcard $(MID_SUBDIR)/*.mid)
MID_OBJS := $(patsubst $(MID_SUBDIR)/%.mid,$(MID_BUILDDIR)/%.o,$(MID_SRCS))

ENUM_ASM_SRCS := $(wildcard include/*.h)
ENUM_ASM_HEADERS := $(patsubst include/%.h,$(ASM_ENUM_INCLUDE_DIR)/%.inc,$(ENUM_ASM_SRCS))

OBJS := $(C_OBJS) $(ASM_OBJS) $(DATA_ASM_OBJS) $(SONG_OBJS) $(MID_OBJS)
OBJS_REL := $(patsubst $(OBJ_DIR)/%,%,$(OBJS))

SUBDIRS  := $(sort $(dir $(OBJS) $(ENUM_ASM_HEADERS)))

$(shell mkdir -p $(SUBDIRS))

.PHONY: all setup clean-tools mostlyclean clean tidy tools extractassets buildassets custom

MAKEFLAGS += --no-print-directory

AUTO_GEN_TARGETS :=

# TODO do we really need this extra step just so that the assets are always extracted at first?
all: build/extracted_assets_$(GAME_VERSION)
	 @$(MAKE) target GAME_VERSION=$(GAME_VERSION)

target: $(ROM)
	@$(SHA1) $(BUILD_NAME).sha1

custom: buildassets
	@$(MAKE) target GAME_VERSION=$(GAME_VERSION)

# kept for backwards compat
compare: $(ROM)
	@$(SHA1) $(BUILD_NAME).sha1

setup: tools

# all tools are build at once
# FIXME figure out why make builds multiple times when specifying all tools here
tools: $(GFX)

$(GFX) $(AIF) $(MID) $(SCANINC) $(PREPROC) $(FIX) $(ASSET_PROCESSOR) tools/bin/agb2mid tools/bin/tmc_strings tools/bin/bin2c &:
	mkdir -p tools/cmake-build
	unset CC CXX AS LD LDFLAGS && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=tools -S tools -B tools/cmake-build
	cmake --build tools/cmake-build -j --target install

# Automatically extract binary data
build/extracted_assets_%: $(ASSET_CONFIGS) $(TRANSLATIONS)
	$(ASSET_PROCESSOR) extract $(GAME_VERSION) $(ASSET_BUILDDIR)
	touch $@

# Extract assets to human readable form
extractassets:
	$(ASSET_PROCESSOR) convert $(GAME_VERSION) $(ASSET_BUILDDIR)

# Build the assets from the human readable form
buildassets:
	$(ASSET_PROCESSOR) build $(GAME_VERSION) $(ASSET_BUILDDIR)

mostlyclean: tidy
	rm -f sound/direct_sound_samples/*.bin
	rm -f $(SONG_OBJS) $(MID_SUBDIR)/*.s
	find . \( -iname '*.1bpp' -o -iname '*.4bpp' -o -iname '*.8bpp' -o -iname '*.gbapal' -o -iname '*.lz' -o -iname '*.latfont' -o -iname '*.hwjpnfont' -o -iname '*.fwjpnfont' \) -exec rm {} +
	rm -f $(AUTO_GEN_TARGETS)

clean-tools:
	rm -rf tools/bin
	rm -rf tools/cmake-build

clean: mostlyclean clean-tools

tidy:
	rm -f tmc.gba tmc.elf tmc.map
	rm -f tmc_demo_usa.gba tmc_demo_usa.elf tmc_demo_usa.map
	rm -f tmc_jp.gba tmc_jp.elf tmc_jp.map
	rm -f tmc_demo_jp.gba tmc_demo_jp.elf tmc_demo_jp.map
	rm -f tmc_eu.gba tmc_eu.elf tmc_eu.map
	rm -r build/*


%.s: ;
%.png: ;
%.pal: ;
%.aif: ;

%.1bpp: %.png  ; $(GFX) $< $@
%.4bpp: %.png  ; $(GFX) $< $@
%.8bpp: %.png  ; $(GFX) $< $@
%.gbapal: %.pal ; $(GFX) $< $@
%.gbapal: %.png ; $(GFX) $< $@
%.lz: % ; $(GFX) $< $@
%.rl: % ; $(GFX) $< $@
	cd $(@D) && ../../$(MID) $(<F)
translations/USA.bin: translations/USA.json ; tools/bin/tmc_strings -p --source $< --dest $@ --size 0x499E0
translations/English.bin: translations/English.json ; tools/bin/tmc_strings -p --source $< --dest $@ --size 0x488C0
translations/French.bin: translations/French.json ; tools/bin/tmc_strings -p --source $< --dest $@ --size 0x47A90
translations/German.bin: translations/German.json ; tools/bin/tmc_strings -p --source $< --dest $@ --size 0x42FC0
translations/Spanish.bin: translations/Spanish.json ; tools/bin/tmc_strings -p --source $< --dest $@ --size 0x41930
translations/Italian.bin: translations/Italian.json ; tools/bin/tmc_strings -p --source $< --dest $@ --size 0x438E0

ifeq ($(NODEP),1)
$(C_BUILDDIR)/%.o: c_dep :=
else
$(C_BUILDDIR)/%.o: c_dep = $(shell $(SCANINC) -I include $(C_SUBDIR)/$*.c)
endif

$(C_BUILDDIR)/%.o : $(C_SUBDIR)/%.c $$(c_dep)
	@$(CPP) $(CPPFLAGS) $< -o $(C_BUILDDIR)/$*.i
	$(PREPROC) $(BUILD_NAME) $(C_BUILDDIR)/$*.i charmap.txt | $(CC1) $(CFLAGS) -o $(C_BUILDDIR)/$*.s
	@echo -e "\t.text\n\t.align\t2, 0 @ Don't pad with nop\n" >> $(C_BUILDDIR)/$*.s
	$(AS) $(ASFLAGS) -o $@ $(C_BUILDDIR)/$*.s

ifeq ($(NODEP),1)
$(ASM_BUILDDIR)/%.o: asm_dep :=
else
$(ASM_BUILDDIR)/%.o: asm_dep = $(shell $(SCANINC) -I . $(ASM_SUBDIR)/$*.s)
endif

$(ASM_BUILDDIR)/%.o: $(ASM_SUBDIR)/%.s $$(asm_dep)
	$(PREPROC) $(BUILD_NAME) $< -- $(PREPROC_INC_PATHS) | $(AS) $(ASFLAGS) -o $@

$(ASM_ENUM_INCLUDE_DIR)/%.inc: include/%.h
	$(ENUM_PROCESSOR) $< $(CC) "-D__attribute__(x)=" "-E" "-nostdinc" "-Itools/agbcc" "-Itools/agbcc/include" "-iquote include" > $@

ifeq ($(NODEP),1)
$(DATA_ASM_BUILDDIR)/%.o: data_dep :=
else
$(DATA_ASM_BUILDDIR)/%.o: data_dep = $(shell $(SCANINC) -I . -I $(ASSET_SUBDIR) -I $(ASSET_BUILDDIR) -I $(ASM_ENUM_INCLUDE_DIR) $(DATA_ASM_SUBDIR)/$*.s)
endif

$(DATA_ASM_BUILDDIR)/%.o: $(DATA_ASM_SUBDIR)/%.s $$(data_dep) $(ENUM_ASM_HEADERS)
	$(PREPROC) $(BUILD_NAME) $< charmap.txt -- $(PREPROC_INC_PATHS) | $(CPP) -I include -nostdinc -undef -Wno-unicode - | $(AS) $(ASFLAGS) -o $@

$(SONG_BUILDDIR)/%.o: $(SONG_SUBDIR)/%.s
	$(AS) $(ASFLAGS) -I sound -o $@ $<

$(OBJ_DIR)/linker.ld: linker.ld
	$(CPP) $(CPPFLAGS) -x c linker.ld | grep -v '^#' >$(OBJ_DIR)/linker.ld

$(ELF): $(OBJS) $(OBJ_DIR)/linker.ld
	cd $(OBJ_DIR) && $(LD) $(LDFLAGS) -n -T linker.ld -o ../../$@ $(LIB)
	$(FIX) $@ -t"$(TITLE)" -c$(GAME_CODE) -m$(MAKER_CODE) -r$(REVISION) --silent

$(ROM): $(ELF)
	$(OBJCOPY) -O binary --gap-fill 0xFF --pad-to 0x9000000 $< $@

usa: ; @$(MAKE) GAME_VERSION=USA
demo_usa: ; @$(MAKE) GAME_VERSION=DEMO_USA
jp: ; @$(MAKE) GAME_VERSION=JP
demo_jp: ; @$(MAKE) GAME_VERSION=DEMO_JP
eu: ; @$(MAKE) GAME_VERSION=EU
