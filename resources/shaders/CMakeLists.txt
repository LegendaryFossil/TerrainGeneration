set(SHADERS
	"quad.vert"
	"quad.frag"
	"skybox.vert"
	"skybox.frag"
	"terrain.vert"
	"terrain.tesc"
	"terrain.tese"
	"terrain.frag"
	"terrainDebug.frag"
	"terrainDebug.tesc"
	"terrainDebug.tese"
	"water.vert"
	"water.frag"
)

install(FILES ${SHADERS} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/shaders")