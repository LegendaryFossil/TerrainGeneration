option(glew-cmake_BUILD_MULTI_CONTEXT "Build the multi context glew library" OFF)
option(glew-cmake_BUILD_SINGLE_CONTEXT "Build the single context glew library" ON)  
option(glew-cmake_BUILD_SHARED "Build the shared glew library" OFF )
option(glew-cmake_BUILD_STATIC "Build the static glew library" ON )
add_subdirectory(glew-2.1.0)

option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
add_subdirectory(glfw-3.3.2)

add_subdirectory(glm-0.9.9.7)