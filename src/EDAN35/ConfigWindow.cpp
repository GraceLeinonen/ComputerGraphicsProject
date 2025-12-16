#include "ConfigWindow.h"

#include <imgui.h>
#include "core/Bonobo.h"

Config::Config(TerrainGrid* grid) {
	terrain = grid;

	// Initialize the default values
	// Get the current values of the terrain
	terrain_dimensions = grid->get_dimensions();
	terrain_scale = grid->get_scale();

	sculpter_size = 3.0f;

	pd_show_points_debugger = false; // pd_ = points_debugger_
	pd_point_size = 20.0f;
	pd_show_single_slice = false;
	pd_single_slice_axis = 0; // 0 = x, 1 = y, 2 = z
	pd_single_slice = terrain_dimensions.y / 2; // Default to the middle slice

	show_sculpting_rays = false;
	crosshair_size = 4.0f;
	show_crosshair = true;

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

	if (opened) {
		// InputInt3 returns a boolean indicating whether the value was changed
		if (ImGui::InputInt3("Terrain Dimensions", &terrain_dimensions.x)) {
			// Minimize the values to 1, and resize the grid
			terrain->resize(max(terrain_dimensions, glm::ivec3(1)));
		}

		if (ImGui::SliderFloat("Terrain Scale", &terrain_scale, 0.0f, 10.0f)) {
			terrain->set_scale(terrain_scale);
		}

		if (ImGui::Button("Clear Terrain")) {
			LogInfo("Cleared terrain");
			terrain->clear();
		}

		ImGui::Separator();

		ImGui::Text("Use Z/X to add/remove terrain at mouse position");
		ImGui::SliderFloat("Sculpting Brush Size", &sculpter_size, 1.0f, 20.0f);

		ImGui::Separator();

		// Add UI to change the seed and scale
		ImGui::InputInt("Perlin Noise Seed", &pn_seed);
		if (ImGui::SliderFloat("Perlin Noise Scale", &pn_scale, 0.01f, 1.0f)) {
			terrain->regenerate(PerlinNoise(pn_seed, pn_scale));
		}

		// If the button is pressed, generate new perlin terrain with the given seed and scale
		if (ImGui::Button("Generate Perlin Terrain")) {
			terrain->regenerate(PerlinNoise(pn_seed, pn_scale));
		}

		bool const DebuggerOpened = ImGui::CollapsingHeader("Debugger");
		if (DebuggerOpened) {
			ImGui::Checkbox("Show sculpting rays", &show_sculpting_rays);
			if (show_sculpting_rays) {
				ImGui::Text("Drawing ray in colour, drawing ray at scale = 1.0 in white");
			}

			ImGui::Separator();


			ImGui::Checkbox("Show Crosshair", &show_crosshair);
			if (show_crosshair) {
				ImGui::SliderFloat("Crosshair Size", &crosshair_size, 1.0f, 16.0f);
			}

			ImGui::Separator();

			ImGui::Checkbox("Show points debugger", &pd_show_points_debugger);
			if (pd_show_points_debugger) {
				ImGui::SliderFloat("Point size", &pd_point_size, 1.0f, 50.0f);
				ImGui::Checkbox("Show only 1 slice", &pd_show_single_slice);

				if (pd_show_single_slice) {
					terrain->setDebugPointsRange(pointsDebuggerRange().first, pointsDebuggerRange().second);

					if (ImGui::Button("X")) {
						pd_single_slice_axis = 0;
					}
					ImGui::SameLine();
					if (ImGui::Button("Y")) {
						pd_single_slice_axis = 1;
					}
					ImGui::SameLine();
					if (ImGui::Button("Z")) {
						pd_single_slice_axis = 2;
					}

					int max_slice = (pd_single_slice_axis == 0 ? terrain_dimensions.x - 1 : (pd_single_slice_axis == 1 ? terrain_dimensions.y - 1 : terrain_dimensions.z - 1));
					if (pd_single_slice < 0) pd_single_slice = 0;
					if (pd_single_slice > max_slice) pd_single_slice = max_slice;

					ImGui::SliderInt("Slice index", &pd_single_slice, 0, max_slice);
				} else {
					terrain->setDebugPointsRange(glm::vec3(0), terrain->get_dimensions());
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
