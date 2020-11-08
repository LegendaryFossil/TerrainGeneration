SET(TERRAIN_DUDV_PATH "${PROJECT_SOURCE_DIR}/resources/textures/terrain")

set(TEXTURES
"${TERRAIN_DUDV_PATH}/grass.png"
"${TERRAIN_DUDV_PATH}/sand.png"
)

install(FILES ${TEXTURES} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/textures/terrain")