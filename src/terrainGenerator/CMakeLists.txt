set(NAME "TerrainGenerator")

set(SRC
	"camera.cpp"
	"camera.h"
	"meshGenerator.cpp"
	"meshGenerator.h"
	"noiseMapGenerator.cpp"
	"noiseMapGenerator.h"
	"perlinNoise.cpp"
	"perlinNoise.h"
	"shaderLoader.cpp"
	"shaderLoader.h"
	"terrainDefs.h"
	"terrainGenerator.cpp"
	"textureGenerator.cpp"
	"textureGenerator.h"
)

set(IMGUI_PATH ImGui)
set(IMGUI_SRC
	"${IMGUI_PATH}/imconfig.h"
	"${IMGUI_PATH}/imgui.cpp"
	"${IMGUI_PATH}/imgui.h"
	"${IMGUI_PATH}/imgui_demo.cpp"
	"${IMGUI_PATH}/imgui_draw.cpp"
	"${IMGUI_PATH}/imgui_impl_glfw.cpp"
	"${IMGUI_PATH}/imgui_impl_glfw.h"
	"${IMGUI_PATH}/imgui_impl_opengl3.cpp"
	"${IMGUI_PATH}/imgui_impl_opengl3.h"
	"${IMGUI_PATH}/imgui_internal.h"
	"${IMGUI_PATH}/imgui_widgets.cpp"
	"${IMGUI_PATH}/imstb_rectpack.h"
	"${IMGUI_PATH}/imstb_textedit.h"
	"${IMGUI_PATH}/imstb_truetype.h"
)

add_compile_options("/std:c++latest")

set(LIBRARIES
	"libglew_static"
	"glfw"
	"glm::glm"
)
    
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC} ${IMGUI_SRC})
source_group("" FILES ${SRC} ${IMGUI_SRC})

add_executable(${NAME} "")
target_sources(${NAME} PRIVATE ${SRC} ${IMGUI_SRC})

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${NAME} PUBLIC ${LIBRARIES})

install(TARGETS ${NAME} DESTINATION ${TERRAIN_GENERATOR_EXE_PATH})