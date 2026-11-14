# Nintendo 3DS homebrew build
TARGET      := NSMBHD-Shop
BUILD       := build
SOURCES     := source
DATA        :=
INCLUDES    := include

APP_TITLE       := NSMBHD Shop
APP_DESCRIPTION := New Super Mario Bros. community mod browser
APP_AUTHOR      := ToastLinus
APP_VERSION     := 1.0.0

CFLAGS   := -g -Wall -O2 -mword-relocations -ffunction-sections -fdata-sections
CFLAGS   += -D__3DS__ -I$(DEVKITPRO)/libctru/include
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS  := -specs=3dsx.specs -g -Wl,--gc-sections
LIBS     := -lctru
LIBDIRS  := $(DEVKITPRO)/libctru/lib

ifneq ($(BUILD),$(notdir $(CURDIR)))
.PHONY: all clean
all:
	@mkdir -p $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@rm -rf $(BUILD)
else
ROOT := $(abspath ..)
include $(DEVKITARM)/3ds_rules

CFILES := $(notdir $(wildcard $(ROOT)/$(SOURCES)/*.c))
OFILES := $(CFILES:.c=.o)
VPATH  := $(ROOT)/$(SOURCES)

.PHONY: all clean
all: $(TARGET).cia

$(TARGET).elf: $(OFILES)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(TARGET).smdh:
	bannertool makesmdh -s "$(APP_TITLE)" -l "$(APP_DESCRIPTION)" -p "$(APP_AUTHOR)" -o $@

$(TARGET).bnr:
	bannertool makebanner -i $(DEVKITPRO)/tools/3dslink/banner.png -o $@

$(TARGET).cia: $(TARGET).elf $(TARGET).smdh $(TARGET).bnr
	makerom -f cia -o $@ -elf $(TARGET).elf -icon $(TARGET).smdh -banner $(TARGET).bnr

clean:
	@rm -f *.o *.elf *.3dsx *.cia *.smdh *.bnr
endif
