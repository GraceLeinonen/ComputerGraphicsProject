#include "main.hpp"
#include "TerrainGrid.h"
#include "ConfigWindow.h"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <clocale>
#include <cstdlib>
#include <stdexcept>


Project::ProjectWrapper::ProjectWrapper(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
		static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
		0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0 };

	window = mWindowManager.CreateGLFWWindow("EDAN35 Final Project", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}

	bonobo::init();
}

Project::ProjectWrapper::~ProjectWrapper()
{
	bonobo::deinit();
}

void
Project::ProjectWrapper::run()
{
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 1.0f, 9.0f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mMovementSpeed = glm::vec3(3.0f); // 3 m/s => 10.8 km/h

	ShaderProgramManager shader_manager;

	// Load the shader used for rendering the debug points
	GLuint debug_point_shader = 0u;
	shader_manager.CreateAndRegisterProgram(
		"triangle_shader",
		{ { ShaderType::vertex,   "common/DebugPointShader.vert" },
		  { ShaderType::fragment, "common/DebugPointShader.frag" } },
		debug_point_shader
	);

	if (debug_point_shader == 0u)
		throw std::runtime_error("Failed to load debug_point_shader");
	shader_manager.ReloadAllPrograms();

	// Create the TerrainGrid (Which is the 3d Voxel grid representing the terrain)
	TerrainGrid* grid = new TerrainGrid(glm::ivec3(10), 1.0f);

	//
	// Create the Debug Points VBO/VAO
	//
	std::pair<GLuint, GLuint> debug_points = grid->debugPointsVBO();
	GLuint debug_points_vao = debug_points.first;
	GLuint debug_points_vbo = debug_points.second;

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Change the clear colour to make it a bit easier to see dark colours

	auto lastTime = std::chrono::high_resolution_clock::now();

	float elapsed_time_s = 0.0f;
	bool show_logs = true;
	bool show_gui = true;
	std::int32_t program_index = 0;

	// Create the Config, which is used to manage the Scene Controls window
	Config* config = new Config(grid);
	
	while (!glfwWindowShouldClose(window)) {

		//
		// CAMERA MOVEMENT/GUI INPUT HANDLING
		//

		auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		lastTime = nowTime;

		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(deltaTimeUs, inputHandler);
		elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();

		if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
			mWindowManager.ToggleFullscreenStatusForWindow(window);


		// Retrieve the actual framebuffer size: for HiDPI monitors,
		// you might end up with a framebuffer larger than what you
		// actually asked for. For example, if you ask for a 1920x1080
		// framebuffer, you might get a 3840x2160 one instead.
		// Also it might change as the user drags the window between
		// monitors with different DPIs, or if the fullscreen status is
		// being toggled.
		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);

		mWindowManager.NewImGuiFrame();

		// Clear the previous frame from the buffer
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		//
		// START RENDERING THE FRAME
		//

		// Get the projection matrix
		glm::mat4 projection = mCamera.GetWorldToClipMatrix();

		// Render the debug points
		if (config->pd_show_points_debugger) {
			glUseProgram(debug_point_shader); // Use the debug point shader
			// Provide the projection matrix to the shader
			glUniformMatrix4fv(glGetUniformLocation(debug_point_shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(debug_points_vao);
			glPointSize(config->pd_point_size);

			auto renderRange = config->pointsDebuggerRange(); // the range of indexes to render
			glm::vec3 renderSize = renderRange.second - renderRange.first; // Dimensions of what to render
			glDrawArrays(GL_POINTS, 0, renderSize.x * renderSize.y * renderSize.z);
			glBindVertexArray(0);
			glUseProgram(0);
		}


		// Draw the scene controls window
		config->draw_config();
		
		// If the terrain is changed (by the user in the config window), update the grid and recreate the VAO/VBOs
		if (config->terrain_updated) {
			auto dimensions = config->pointsDebuggerRange();
			std::pair<GLuint, GLuint> debug_points = grid->debugPointsVBOWithDimensions(dimensions.first, dimensions.second);
			debug_points_vao = debug_points.first;
			debug_points_vbo = debug_points.second;
		}


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		// Render the basis (if enabled)
		if (config->bd_show_basis)
			bonobo::renderBasis(config->bd_thickness, config->bd_length, mCamera.GetWorldToClipMatrix());
		// Render the logs window (if enabled)
		if (show_logs)
			Log::View::Render();
		// Render the ImGui windows
		mWindowManager.RenderImGuiFrame(show_gui);

		glfwSwapBuffers(window);
	}
}

int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		Project::ProjectWrapper assignment2(framework.GetWindowManager());
		assignment2.run();
	}
	catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
