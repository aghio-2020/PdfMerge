# Detect OS
ifeq ($(OS),Windows_NT)
    PLATFORM = windows
    PDFIO_LIB = pdfio1
else
    PLATFORM = linux
    PDFIO_LIB = pdfio
endif

CFLAGS = -std=c++17 -I$(CURDIR)/dependencies/$(PLATFORM)/include
PDFIO_LIBDIR = $(CURDIR)/dependencies/$(PLATFORM)/lib

SOURCES = main.cpp
OBJ_DIR = $(CURDIR)/obj
OBJS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))

OUTPUT_DIR = $(CURDIR)/output
TARGET = ${OUTPUT_DIR}/pdfMerge

all: $(TARGET)

$(TARGET): $(OBJS) $(OUTPUT_DIR)
	g++ $(CFLAGS) -o $(TARGET) $(OBJS) -L$(PDFIO_LIBDIR) -Wl,-rpath,$(PDFIO_LIBDIR) -l$(PDFIO_LIB) -lz

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	g++ $(CFLAGS) -c $< -o $@


$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)

clean:
	rm -rf $(OBJ_DIR) $(OUTPUT_DIR)
