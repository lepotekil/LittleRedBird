CC = cl
CFLAGS = /nologo /DNDEBUG /D_CRT_SECURE_NO_WARNINGS /Ox /MT /W4 /EHsc /Iincludes

LDFLAGS = /link Advapi32.lib User32.lib

OUTPUT_DIR = output
TARGET = $(OUTPUT_DIR)\littleredbird.exe
SOURCE = main.c

all: $(OUTPUT_DIR) $(TARGET)

$(OUTPUT_DIR):
	@if not exist "$(OUTPUT_DIR)" mkdir "$(OUTPUT_DIR)"

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) /Fe:$(TARGET) $(LDFLAGS)

clean:
	@if exist "$(OUTPUT_DIR)" rmdir /s /q "$(OUTPUT_DIR)"

.PHONY: all clean