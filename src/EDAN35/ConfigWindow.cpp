#include "ConfigWindow.h"

#include <imgui.h>
#include "core/Bonobo.h"

Config::Config(TerrainGrid* grid) {
	terrain = grid;

	// Initialize the default values
	// Get the current values of the terrain
	terrain_dimensions = grid->get_dimensions();
	terrain_updated = false;
	terrain_scale = grid->get_scale();

	pd_show_points_debugger = false; // pd_ = points_debugger_
	pd_point_size = 20.0f;
	pd_show_single_slice = false;
	pd_single_slice_axis = 0; // 0 = x, 1 = y, 2 = z
	pd_single_slice = terrain_dimensions.y / 2; // Default to the middle slice

	bd_show_basis = false; // bd_ = basis_debugger_
	bd_thickness = 1.0f;
	bd_length = 3.0f;

	// Get the current config values of the perlin noise
	PerlinNoise noise = grid->getNoise();
	pn_seed = noise.getSeed(); // pn_ = perlin_noise_
	pn_scale = noise.getScale();
}


void Config::draw_config() {
	bool const opened = ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_None);
	terrain_updated = false;

	if (opened) {
		// InputInt3 returns a boolean indicating whether the value was changed
		bool terrain_dimensions_updated = ImGui::InputInt3("Terrain Dimensions", &terrain_dimensions.x);
		terrain_updated = terrain_updated || terrain_dimensions_updated; // Save any changes to the terrain for the main loop

		if (terrain_dimensions_updated) {
			// Minimize the values to 1, and resize the grid
			terrain->resize(max(terrain_dimensions, glm::ivec3(1)));
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

		// Add UI to change the seed and scale
		ImGui::InputInt("Perlin Noise Seed", &pn_seed);
		if (ImGui::SliderFloat("Perlin Noise Scale", &pn_scale, 0.01f, 1.0f)) {
			terrain->regenerate(PerlinNoise(pn_seed, pn_scale));
			terrain_updated = true;
		}

		// If the button is pressed, generate new perlin terrain with the given seed and scale
		if (ImGui::Button("Generate Perlin Terrain")) {
			terrain->regenerate(PerlinNoise(pn_seed, pn_scale));
			terrain_updated = true;
		}

		bool const DebuggerOpened = ImGui::CollapsingHeader("Debugger");
		if (DebuggerOpened) {
			ImGui::Checkbox("Show points debugger", &pd_show_points_debugger);
			if (pd_show_points_debugger) {
				ImGui::SliderFloat("Point size", &pd_point_size, 1.0f, 50.0f);
				if (ImGui::Checkbox("Show only 1 slice", &pd_show_single_slice)) terrain_updated = true;
				if (pd_show_single_slice) {
					if (ImGui::Button("X")) {
						pd_single_slice_axis = 0;
						terrain_updated = true;
					}
					ImGui::SameLine();
					if (ImGui::Button("Y")) {
						pd_single_slice_axis = 1;
						terrain_updated = true;
					}
					ImGui::SameLine();
					if (ImGui::Button("Z")) {
						pd_single_slice_axis = 2;
						terrain_updated = true;
					}

					int max_slice = (pd_single_slice_axis == 0 ? terrain_dimensions.x - 1 : (pd_single_slice_axis == 1 ? terrain_dimensions.y - 1 : terrain_dimensions.z - 1));
					if (pd_single_slice < 0) pd_single_slice = 0;
					if (pd_single_slice > max_slice) pd_single_slice = max_slice;

					if (ImGui::SliderInt("Slice index", &pd_single_slice, 0, max_slice)) {
						terrain_updated = true;
					}
				}
			}
			ImGui::Separator();
			ImGui::Checkbox("Show basis", &bd_show_basis);
			if (bd_show_basis) {
				ImGui::SliderFloat("Basis thickness scale", &bd_thickness, 0.0f, 100.0f);
				ImGui::SliderFloat("Basis length scale", &bd_length, 0.0f, 100.0f);
			}
		}
	}
	ImGui::End();
}


std::pair<glm::ivec3, glm::ivec3> Config::pointsDebuggerRange() const {
	if (pd_show_single_slice) {
		int minX = (pd_single_slice_axis == 0) ? pd_single_slice : 0;
		int minY = (pd_single_slice_axis == 1) ? pd_single_slice : 0;
		int minZ = (pd_single_slice_axis == 2) ? pd_single_slice : 0;

		int maxX = (pd_single_slice_axis == 0) ? pd_single_slice + 1 : terrain->get_x_size();
		int maxY = (pd_single_slice_axis == 1) ? pd_single_slice + 1 : terrain->get_y_size();
		int maxZ = (pd_single_slice_axis == 2) ? pd_single_slice + 1 : terrain->get_z_size();

		return { glm::ivec3(minX, minY, minZ), glm::ivec3(maxX, maxY, maxZ) };
	}
	else {
		return { glm::ivec3(0, 0, 0), terrain->get_dimensions() };
	}
}
