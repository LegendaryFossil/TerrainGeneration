SET(WATER_NORMAL_MAP_PATH "${PROJECT_SOURCE_DIR}/resources/textures/waterNormalMap")

set(TEXTURES
"${WATER_NORMAL_MAP_PATH}/waterNormalMap1.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap2.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap3.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap4.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap5.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap6.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap7.png"
)

install(FILES ${TEXTURES} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/textures/waterNormalMap")