set(SHADERS
	"debugMesh.frag"
	"debugMesh.vert"
	"terrainGenerator.frag"
	"terrainGenerator.vert"
	"terrainTest.vert"
	"terrainTest.tesc"
	"terrainTest.tese"
	"terrainTest.frag"
)

install(FILES ${SHADERS} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/shaders")