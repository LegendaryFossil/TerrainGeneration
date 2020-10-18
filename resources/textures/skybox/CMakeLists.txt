SET(SKYBOX_PATH "${PROJECT_SOURCE_DIR}/resources/textures/skybox")

set(TEXTURES
"${SKYBOX_PATH}/back.jpg"
"${SKYBOX_PATH}/front.jpg"
"${SKYBOX_PATH}/left.jpg"
"${SKYBOX_PATH}/right.jpg"
"${SKYBOX_PATH}/bottom.jpg"
"${SKYBOX_PATH}/top.jpg"
)

install(FILES ${TEXTURES} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/textures/skybox")