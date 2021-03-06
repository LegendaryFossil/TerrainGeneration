set(NAME "TerrainGenerator")

set(SRC
	"camera.cpp"
	"camera.h"
	"lightDefs.h"
	"meshGenerator.cpp"
	"meshGenerator.h"
	"noiseMapGenerator.cpp"
	"noiseMapGenerator.h"
	"falloffMapGenerator.cpp"
	"falloffMapGenerator.h"
	"sceneControl.cpp"
	"sceneControl.h"
	"sceneRendering.cpp"
	"sceneRendering.h"
	"sceneShaders.cpp"
	"sceneShaders.h"
	"sceneDefs.h"
	"shaderLoader.cpp"
	"shaderLoader.h"
	"terrainDefs.h"
	"terrainGenerator.cpp"
	"sceneUI.cpp"
	"sceneUI.h"
	"textureGenerator.cpp"
	"textureGenerator.h"
	"timeMeasureUtils.cpp"
	"timeMeasureUtils.h"
	"uniformDefs.h"
	"utils.cpp"
	"utils.h"
	"windowDefs.h"
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

set(FastNoiseSIMD_PATH FastNoiseSIMD/FastNoiseSIMD)
set(FastNoiseSIMD_SRC
	"${FastNoiseSIMD_PATH}/FastNoiseSIMD.h"
	"${FastNoiseSIMD_PATH}/FastNoiseSIMD.cpp"
	"${FastNoiseSIMD_PATH}/FastNoiseSIMD_avx2.cpp"
	"${FastNoiseSIMD_PATH}/FastNoiseSIMD_avx512.cpp"
	"${FastNoiseSIMD_PATH}/FastNoiseSIMD_internal.cpp"
	"${FastNoiseSIMD_PATH}/FastNoiseSIMD_internal.h"
	"${FastNoiseSIMD_PATH}/FastNoiseSIMD_neon.cpp"
	"${FastNoiseSIMD_PATH}/FastNoiseSIMD_sse2.cpp"
	"${FastNoiseSIMD_PATH}/FastNoiseSIMD_sse41.cpp"
)

set_source_files_properties("${FastNoiseSIMD_PATH}/FastNoiseSIMD_avx2.cpp" PROPERTIES COMPILE_FLAGS /arch:AVX2)

set(FastNoise_PATH FastNoise)
set(FastNoise_SRC
	"${FastNoise_PATH}/FastNoise.h"
	"${FastNoise_PATH}/FastNoise.cpp"
)

add_compile_options("/std:c++latest")

set(LIBRARIES
	"libglew_static"
	"glfw"
	"glm::glm"
	"stb_image"
)
    
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC} ${IMGUI_SRC} ${FastNoiseSIMD_SRC} ${FastNoise_SRC})
source_group("" FILES ${SRC} ${IMGUI_SRC} ${FastNoiseSIMD_SRC} ${FastNoise_SRC})

add_executable(${NAME} "")
target_sources(${NAME} PRIVATE ${SRC} ${IMGUI_SRC} ${FastNoiseSIMD_SRC} ${FastNoise_SRC})

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD)
target_link_libraries(${NAME} PUBLIC ${LIBRARIES})

install(TARGETS ${NAME} DESTINATION ${TERRAIN_GENERATOR_EXE_PATH})