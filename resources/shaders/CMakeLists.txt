set(SHADERS
	"terrain.vert"
	"terrain.tesc"
	"terrain.tese"
	"terrain.frag"
	"terrainDebug.frag"
	"terrainDebug.tesc"
	"terrainDebug.tese"
)

install(FILES ${SHADERS} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/shaders")