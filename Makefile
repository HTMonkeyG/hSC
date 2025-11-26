MAKEFLAGS += -s -j

DIST_DIR = ./dist
SRC_DIR = ./src

SRC_DIRS = $(SRC_DIR) $(wildcard $(SRC_DIR)/*/)

C_SRC = $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*/*.c)
CPP_SRC = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/*/*.cpp)

C_OBJ = $(addprefix $(DIST_DIR)/, $(notdir $(C_SRC:.c=.o)))
CPP_OBJ = $(addprefix $(DIST_DIR)/, $(notdir $(CPP_SRC:.cpp=.o)))

VERSION_SCRIPT = $(SRC_DIR)/exports.txt

TARGET = hsc-main.dll
BIN_TARGET = $(DIST_DIR)/$(TARGET)

# Compiler paths.
CC = gcc
CXX = g++

# Params.
CFLAGS = -O3 -ffunction-sections -fdata-sections -static -flto=auto -s -mavx -msse
CFLAGS += -Wall -Wformat -Wno-unused-const-variable -Wno-unused-function -Wno-missing-braces
CFLAGS += -I./src
# Include ImGui.
CFLAGS += -I./libraries/htmodloader/includes/imgui-1.92.2b -I./libraries/htmodloader/includes/imgui-1.92.2b/backends
# Include HTML.
CFLAGS += -I./libraries/htmodloader/includes/htmodloader

LFLAGS = -Wl,--gc-sections,-O3,--as-needed,--version-script=$(VERSION_SCRIPT)
LFLAGS += -lstdc++
LFLAGS += -L./libraries/htmodloader/lib -lhtmodloader

vpath %.c $(SRC_DIRS)
vpath %.cpp $(SRC_DIRS)

.PHONY: all clean libs clean_libs clean_all

all: $(BIN_TARGET)

$(BIN_TARGET): $(C_OBJ) $(CPP_OBJ)
	@echo Linking ...
	@$(CXX) $(CFLAGS) $^ -shared -o $@ $(LFLAGS)
	@echo Done.

$(DIST_DIR)/%.o: %.c
	@echo Compiling file "$<" ...
	@$(CC) $(CFLAGS) -c $< -o $@

$(DIST_DIR)/%.o: %.cpp
	@echo Compiling file "$<" ...
	@$(CXX) $(CFLAGS) -c $< -o $@

clean:
	-@del .\dist\*.o
	-@del .\dist\*.dll
