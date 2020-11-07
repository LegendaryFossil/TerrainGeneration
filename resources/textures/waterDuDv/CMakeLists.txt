SET(WATER_DUDV_PATH "${PROJECT_SOURCE_DIR}/resources/textures/waterDuDv")

set(TEXTURES
"${WATER_DUDV_PATH}/waterDuDv1.png"
"${WATER_DUDV_PATH}/waterDuDv2.png"
"${WATER_DUDV_PATH}/waterDuDv3.png"
"${WATER_DUDV_PATH}/waterDuDv4.png"
"${WATER_DUDV_PATH}/waterDuDv5.png"
"${WATER_DUDV_PATH}/waterDuDv6.png"
"${WATER_DUDV_PATH}/waterDuDv7.png"
)

install(FILES ${TEXTURES} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/textures/waterDuDv")