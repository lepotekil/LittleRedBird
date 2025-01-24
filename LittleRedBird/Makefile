CC=cl
CFLAGS=/nologo /DNDEBUG /Ox /MT /W4 /GS- /Tp /I"includes"
LDFLAGS=/link Advapi32.lib User32.lib
SRC_DIR=src
INCLUDES_DIR=includes
OUTPUT_DIR=output
TARGET=$(OUTPUT_DIR)/littleredbird.exe
SOURCES=$(SRC_DIR)/main.c $(SRC_DIR)/utils.c

all: $(OUTPUT_DIR) $(TARGET)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) /Fe:"$@" /Fo"$(OUTPUT_DIR)/" $(LDFLAGS)
	del $(OUTPUT_DIR)\*.obj

clean:
	rm -rf $(OUTPUT_DIR)
	del *.obj

.PHONY: all clean