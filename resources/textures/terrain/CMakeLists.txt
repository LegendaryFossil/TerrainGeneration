SET(TERRAIN_DUDV_PATH "${PROJECT_SOURCE_DIR}/resources/textures/terrain")

set(TEXTURES
"${TERRAIN_DUDV_PATH}/grass.png"
"${TERRAIN_DUDV_PATH}/mountain.png"
"${TERRAIN_DUDV_PATH}/rock.png"
"${TERRAIN_DUDV_PATH}/sand.png"
"${TERRAIN_DUDV_PATH}/snow.png"
)

install(FILES ${TEXTURES} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/textures/terrain")