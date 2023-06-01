#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "implot.h"
#include "implot_internal.h"

void gui_editor(bool &run_sim, bool &classical){
    ImGui::Text("This is a test");
    ImGui::Checkbox("Run Simulation", &run_sim);
    if(!run_sim)
        ImGui::Checkbox("Classical System", &classical);
}