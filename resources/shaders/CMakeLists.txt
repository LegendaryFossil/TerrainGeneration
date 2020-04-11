set(SHADERS
	"debugMesh.frag"
	"debugMesh.vert"
	"terrainGenerator.frag"
	"terrainGenerator.vert"
)

install(FILES ${SHADERS} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/shaders")