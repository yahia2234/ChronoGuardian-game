cmake_minimum_required(VERSION 3.15)
project(ChronoGuardian)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

# GLM (header-only library)
find_package(glm REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Input.cpp
    src/Player.cpp
    src/GameObject.cpp
    src/Physics.cpp
    src/Level.cpp
    src/Level1.cpp
    src/Level2.cpp
    src/Renderer.cpp
    src/Texture.cpp
    src/Mesh.cpp
    src/ParticleSystem.cpp
    src/AudioManager.cpp
)


# Header files
set(HEADERS
    include/Game.h
    include/Shader.h
    include/Camera.h
    include/Input.h
    include/Player.h
    include/Physics.h
    include/Level.h
    include/Level1.h
    include/Level2.h
    include/Renderer.h
    include/Texture.h
    include/Mesh.h
    include/ParticleSystem.h
    include/AudioManager.h
    include/GameObject.h
    include/Transform.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    GLUT::GLUT
    GLEW::GLEW
    glm::glm
)

# Add OpenAL for audio (macOS has it built-in)
if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework OpenAL")
else()
    find_package(OpenAL REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${OPENAL_LIBRARY})
    target_include_directories(${PROJECT_NAME} PRIVATE ${OPENAL_INCLUDE_DIR})
endif()


# Copy shaders to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
