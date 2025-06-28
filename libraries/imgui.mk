DIST_DIR = ./dist
IMGUI_DIR = ./libraries/imgui

# Imgui sources and objects
IMGUI_SRC = $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
IMGUI_OBJ = $(patsubst $(IMGUI_DIR)/%.cpp, $(DIST_DIR)/imgui/%.o, $(IMGUI_SRC))

IMGUI_IMPL_WIN32_SRC = $(IMGUI_DIR)/backends/imgui_impl_win32.cpp
IMGUI_IMPL_WIN32_OBJ = $(patsubst $(IMGUI_DIR)/backends/%.cpp, $(DIST_DIR)/imgui/backends/%.o, $(IMGUI_IMPL_WIN32_SRC))

IMGUI_IMPL_DX12_SRC = $(IMGUI_DIR)/backends/imgui_impl_dx12.cpp
IMGUI_IMPL_DX12_OBJ = $(patsubst $(IMGUI_DIR)/backends/%.cpp, $(DIST_DIR)/imgui/backends/%.o, $(IMGUI_IMPL_DX12_SRC))

CXX = g++
AR = ar
CFLAGS = -std=c++11 -g -Wall -Wformat -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -DNDEBUG

.PHONY: all clean prepare

all: prepare $(DIST_DIR)/libimgui.a $(DIST_DIR)/libimgui_impl_win32.a $(DIST_DIR)/libimgui_impl_dx12.a

prepare:
	@mkdir -p $(DIST_DIR)/imgui/backends

# Rule to compile imgui sources
$(DIST_DIR)/imgui/%.o: $(IMGUI_DIR)/%.cpp | prepare
	@echo Compiling $< ...
	@$(CXX) $(CFLAGS) -c $< -o $@

# Rule to compile imgui backend sources
$(DIST_DIR)/imgui/backends/%.o: $(IMGUI_DIR)/backends/%.cpp | prepare
	@echo Compiling $< ...
	@$(CXX) $(CFLAGS) -c $< -o $@

$(DIST_DIR)/libimgui.a: $(IMGUI_OBJ)
	@$(AR) rcs $@ $^

$(DIST_DIR)/libimgui_impl_win32.a: $(IMGUI_IMPL_WIN32_OBJ)
	@$(AR) rcs $@ $^

$(DIST_DIR)/libimgui_impl_dx12.a: $(IMGUI_IMPL_DX12_OBJ)
	@$(AR) rcs $@ $^

clean:
	-@rm -rf $(DIST_DIR)/imgui
	-@rm -f $(DIST_DIR)/libimgui.a
	-@rm -f $(DIST_DIR)/libimgui_impl_win32.a
	-@rm -f $(DIST_DIR)/libimgui_impl_dx12.a
