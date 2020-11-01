SET(WATER_NORMAL_MAP_PATH "${PROJECT_SOURCE_DIR}/resources/textures/waterNormalMap")

set(TEXTURES
"${WATER_NORMAL_MAP_PATH}/waterNormalMap.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap.png"
"${WATER_NORMAL_MAP_PATH}/waterNormalMap.png"
)

install(FILES ${TEXTURES} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/textures/waterNormalMap")