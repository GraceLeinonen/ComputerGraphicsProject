add_executable (EDAN35_Project)

target_sources (
	EDAN35_Project
	PRIVATE
		"main.hpp"
		"main.cpp"
 "TerrainGrid.cpp" "TerrainGrid.h" "ConfigWindow.cpp" "ConfigWindow.h" "PerlinNoise.cpp" "PerlinNoise.h" "TerrainMesh.cpp" "TerrainMesh.h")

target_link_libraries (EDAN35_Project PRIVATE assignment_setup)

install (TARGETS EDAN35_Project DESTINATION bin)

copy_dlls (EDAN35_Project "${CMAKE_CURRENT_BINARY_DIR}")
