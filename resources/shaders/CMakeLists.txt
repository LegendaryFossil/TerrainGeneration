set(SHADERS
	"light.vert"
	"light.frag"
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
	"waterDebug.vert"
	"waterDebug.frag"
)

install(FILES ${SHADERS} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/shaders")