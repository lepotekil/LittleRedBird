CC=cl
CFLAGS=/nologo /DNDEBUG /Ox /MT /W4 /GS- /Iincludes
LDFLAGS=/link Advapi32.lib User32.lib
OUTPUT_DIR=output
TARGET=$(OUTPUT_DIR)\littleredbird.exe
SOURCE=main.c

all: $(OUTPUT_DIR) $(TARGET)

$(OUTPUT_DIR):
	@if not exist "$(OUTPUT_DIR)" mkdir "$(OUTPUT_DIR)"

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) /Fe:$(TARGET) $(LDFLAGS)

clean:
	@if exist "$(OUTPUT_DIR)" rmdir /s /q "$(OUTPUT_DIR)"

.PHONY: all clean