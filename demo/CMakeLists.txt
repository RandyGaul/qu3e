set(SRCROOT ${CMAKE_CURRENT_SOURCE_DIR})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})


add_executable(demo main.cpp Clock.cpp)
target_link_libraries(demo qu3e ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
