#include "ConfigWindow.h"

#include <imgui.h>
#include "core/Bonobo.h"

Config::Config(TerrainGrid* grid) {
	terrain = grid;

	// Initialize the default values
	terrain_dimensions = glm::vec3(10, 10, 10);
	terrain_updated = false;
	terrain_scale = grid->get_scale();

	pd_show_points_debugger = false; // pd_ = points_debugger_
	pd_point_size = 20.0f;

	md_show_mesh_debugger = false; // md_ = mesh_debugger_

	bd_show_basis = false; // bd_ = basis_debugger_
	bd_thickness = 1.0f;
	bd_length = 3.0f;
}


void Config::draw_config() {
	bool const opened = ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_None);
	terrain_updated = false;

	if (opened) {
		// InputInt3 returns a boolean indicating whether the value was changed
		bool terrain_dimensions_updated = ImGui::InputInt3("Terrain Dimensions", &terrain_dimensions.x);
		terrain_updated = terrain_updated || terrain_dimensions_updated; // Save any changes to the terrain for the main loop

		if (terrain_dimensions_updated) {
			if (terrain_dimensions.x < 1) terrain_dimensions.x = 1;
			if (terrain_dimensions.y < 1) terrain_dimensions.y = 1;
			if (terrain_dimensions.z < 1) terrain_dimensions.z = 1;

			terrain->resize(terrain_dimensions.x, terrain_dimensions.y, terrain_dimensions.z);
		}

		if (ImGui::SliderFloat("Terrain Scale", &terrain_scale, 0.0f, 10.0f)) {
			terrain->set_scale(terrain_scale);
			terrain_updated = true;
		}

		if (ImGui::Button("Clear Terrain")) {
			LogInfo("Cleared terrain");
			terrain_updated = true;
			terrain->clear();
		}

		if (ImGui::Button("Generate Perlin Terrain")) {
			LogInfo("Generated new Perlin terrain");
		}

		bool const DebuggerOpened = ImGui::CollapsingHeader("Debugger");
		if (DebuggerOpened) {
			ImGui::Checkbox("Show points debugger", &pd_show_points_debugger);
			if (pd_show_points_debugger) {
				ImGui::SliderFloat("Point size", &pd_point_size, 1.0f, 50.0f);
			}
			ImGui::Checkbox("Show mesh debugger", &md_show_mesh_debugger);
			ImGui::Checkbox("Show basis", &bd_show_basis);
			if (bd_show_basis) {
				ImGui::SliderFloat("Basis thickness scale", &bd_thickness, 0.0f, 100.0f);
				ImGui::SliderFloat("Basis length scale", &bd_length, 0.0f, 100.0f);
			}
		}
	}
	ImGui::End();
}
