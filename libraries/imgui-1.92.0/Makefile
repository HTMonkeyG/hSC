CXX = g++
AR = ar

CFLAGS = -std=c++11 -g -Wall -Wformat -I. -I./backends -DNDEBUG

all: libimgui.a libimgui_impl_win32.a libimgui_impl_dx11.a libimgui_impl_opengl3.a libimgui_impl_dx9.a libimgui_impl_dx12.a libimgui_demo.a

libimgui.a: ./imgui_draw.o ./imgui_tables.o ./imgui_widgets.o ./imgui.o
	$(AR) rcs $@ $^

libimgui_demo.a: ./imgui_demo.o
	$(AR) rcs $@ $^

libimgui_impl_win32.a: ./imgui_impl_win32.o
	$(AR) rcs $@ $<

libimgui_impl_dx12.a: ./imgui_impl_dx12.o
	$(AR) rcs $@ $<

libimgui_impl_dx11.a: ./imgui_impl_dx11.o
	$(AR) rcs $@ $<

libimgui_impl_dx9.a: ./imgui_impl_dx9.o
	$(AR) rcs $@ $<

libimgui_impl_opengl3.a: ./imgui_impl_opengl3.o
	$(AR) rcs $@ $<

./imgui_draw.o: ./imgui_draw.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

./imgui_tables.o: ./imgui_tables.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

./imgui_widgets.o: ./imgui_widgets.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

./imgui.o: ./imgui.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

./imgui_demo.o: ./imgui_demo.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

./imgui_impl_opengl3.o: ./backends/imgui_impl_opengl3.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

./imgui_impl_dx12.o: ./backends/imgui_impl_dx12.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

./imgui_impl_dx11.o: ./backends/imgui_impl_dx11.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

./imgui_impl_dx9.o: ./backends/imgui_impl_dx9.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

./imgui_impl_win32.o: ./backends/imgui_impl_win32.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	-del *.o
	-del *.a
