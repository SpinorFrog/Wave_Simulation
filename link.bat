@echo Linking Files w/Library
g++ -o app main.o glad.o gui.o backend.o imgui.o imgui_impl_opengl3.o implot.o implot_items.o imgui_impl_glfw.o imgui_impl_win32.o imgui_draw.o imgui_tables.o imgui_widgets.o -L. -lcln -lgmp -lglfw3 -lopengl32 -ld3dcompiler -lgdi32 -ldwmapi
@echo Linking Complete
.\app