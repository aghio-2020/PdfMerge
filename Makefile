CFLAGS = -std=c++17 -Idependencies/include 

PDFIO_LIBDIR = $(CURDIR)/dependencies/pdfio
PDFIO_LIB = pdfio1

SOURCES = main.cpp
OBJ_DIR = $(CURDIR)/obj
OBJS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))

OUTPUT_DIR = $(CURDIR)/output
TARGET = ${OUTPUT_DIR}/pdfMerge

all: $(TARGET)

$(TARGET): $(OBJS) $(OUTPUT_DIR) copy_dlls 
	g++ $(CFLAGS) -L$(PDFIO_LIBDIR) -o $(TARGET) $(OBJS) -l$(PDFIO_LIB)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	g++ $(CFLAGS) -c $< -o $@

copy_dlls:
	cp $(PDFIO_LIBDIR)/pdfio1.dll $(OUTPUT_DIR)
	cp $(PDFIO_LIBDIR)/zlibd.dll $(OUTPUT_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)

clean:
	rm -rf $(OBJ_DIR) $(OUTPUT_DIR)
