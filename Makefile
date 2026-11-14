# Native Nintendo 3DS homebrew build
.SUFFIXES:
include $(DEVKITARM)/3ds_rules

TARGET      := NSMBHD-Shop
BUILD       := build
SOURCES     := source
DATA        := data
INCLUDES    := include
GRAPHICS    :=

APP_TITLE       := NSMBHD Shop
APP_DESCRIPTION := New Super Mario Bros. community mod browser
APP_AUTHOR      := ToastLinus
APP_VERSION    := 1.0.0

CFLAGS      := -g -Wall -O2 -mword-relocations -ffunction-sections \
               -D__3DS__
CFLAGS      += $(INCLUDE) -I$(DEVKITPRO)/libctru/include
CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS     := -g
LDFLAGS     := -specs=3dsx.specs -g -Wl,-Map,$(notdir $*.map)

LIBS        := -lctru
LIBDIRS     := $(LIBCTRU)

ifneq ($(BUILD),$(notdir $(CURDIR)))
.PHONY: all
all:
	@mkdir -p $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
else
.PHONY: all
all: $(TARGET).cia

CFILES      := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
OFILES      := $(CFILES:.c=.o)
VPATH       := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OFILES)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(TARGET).3dsx: $(TARGET).elf
	3dsxtool $< $@ --smdh=$(TARGET).smdh

$(TARGET).smdh:
	bannertool makesmdh -s "$(APP_TITLE)" -l "$(APP_DESCRIPTION)" -p "$(APP_AUTHOR)" -i $(CURDIR)/icon.png -o $@

$(TARGET).cia: $(TARGET).elf $(TARGET).smdh
	 makerom -f cia -o $(TARGET).cia -elf $(TARGET).elf -rsf $(DEVKITPRO)/tools/3dslink/3dslink.rsf -icon $(TARGET).smdh -banner $(DEVKITPRO)/tools/3dslink/banner.bnr

.PHONY: clean
clean:
	rm -f *.o *.elf *.3dsx *.cia *.smdh
endif
