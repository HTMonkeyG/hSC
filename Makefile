MAKEFLAGS += -s -j6

DIST_DIR = ./dist
SRC_DIR = ./src

C_SRC = $(wildcard $(SRC_DIR)/*.c)
CPP_SRC = $(wildcard $(SRC_DIR)/*.cpp)
C_OBJ = $(patsubst %.c, $(DIST_DIR)/%.o, $(notdir $(C_SRC)))
CPP_OBJ = $(patsubst %.cpp, $(DIST_DIR)/%.o, $(notdir $(CPP_SRC)))
VERSION_SCRIPT = $(SRC_DIR)/exports.txt

TARGET = hsc-main.dll
BIN_TARGET = $(DIST_DIR)/$(TARGET)

CC = gcc
CXX = g++

CFLAGS = -Wall -Wformat -Os -ffunction-sections -fdata-sections -static -flto -s -mavx -msse
CFLAGS += -Wl,--gc-sections,--version-script=$(VERSION_SCRIPT)
# Include ImGui.
CFLAGS += -I./libraries/imgui -I./libraries/imgui/backends
# Include MinHook.
CFLAGS += -I./libraries/MinHook/include
# Include kiero.
CFLAGS += -I./libraries/kiero
# Include UGLHook.
CFLAGS += -I./libraries/UGLHook/src

LFLAGS = -lgdi32 -ld3d12 -ldwmapi -ld3dcompiler -lstdc++
LFLAGS += -L./libraries/kiero -lkiero
LFLAGS += -L./libraries/MinHook -lMinHook
LFLAGS += -L$(DIST_DIR) -limgui -limgui_impl_win32 -limgui_impl_dx12
LFLAGS += -L./libraries/UGLHook -luglhook

.PHONY: all clean libs clean_libs clean_all prepare

prepare:
	@-mkdir "$(DIST_DIR)"

$(BIN_TARGET): $(C_OBJ) $(CPP_OBJ)
	@echo Linking ...
	@$(CXX) $(CFLAGS) $^ -shared -o $@ $(LFLAGS)
	@echo Done.

$(DIST_DIR)/%.o: $(SRC_DIR)/%.c | prepare
	@echo Compiling $< ...
	@$(CC) $(CFLAGS) -c $< -o $@

$(DIST_DIR)/%.o: $(SRC_DIR)/%.cpp | prepare
	@echo Compiling $< ...
	@$(CXX) $(CFLAGS) -c $< -o $@

clean_all: clean_libs clean

clean:
	-@del .\dist\*.o
	-@del .\dist\*.dll

all: libs
	-@make $(BIN_TARGET)

libs:
	@echo Compiling libraries ...
	-@make -f ./libraries/imgui.mk all
	-@make -s -C ./libraries/MinHook libMinHook.a
	-@make -s -C ./libraries/UGLHook
	-@make -s -C ./libraries/kiero

clean_libs:
	-@make -f ./libraries/imgui.mk clean
	-@make -s -C ./libraries/kiero clean
	-@make -s -C ./libraries/UGLHook clean
	-@make -s -C ./libraries/MinHook clean
