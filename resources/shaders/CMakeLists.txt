set(SHADERS
	"terrain.vert"
	"terrain.tesc"
	"terrain.tese"
	"terrain.frag"
	"terrainDebug.tese"
	"terrainDebug.frag"
)

install(FILES ${SHADERS} DESTINATION "${TERRAIN_GENERATOR_EXE_PATH}/resources/shaders")