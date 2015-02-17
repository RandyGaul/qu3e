add_definitions(-DFREEGLUT_EXPORTS -DFREEGLUT_STATIC -D_CRT_SECURE_NO_WARNINGS)

set(demo_srcs
	Clock.cpp
	Demo.cpp
	main.cpp
)

set(demo_hdrs
	Demo.h
	DropBoxes.h
	RayPush.h
	BoxStack.h
	Clock.h
)

source_group(Include FILES ${demo_srcs} ${demo_hdrs})

include_directories (
	${OPENGL_INCLUDE_DIR}
	${qu3e_SOURCE_DIR}
)

if(APPLE)
	# We are not using the Apple's framework version, but X11's
	include_directories(/usr/X11/include)
	link_directories(/usr/X11/lib )
	set(OPENGL_LIBRARIES GL GLU GLUT X11)
elseif(WIN32)
	set(ADDITIONAL_LIBRARIES winmm)
endif(APPLE)

add_executable(
	demo
	${demo_srcs}
	${demo_hdrs}
)

target_link_libraries(
	demo
	qu3e
	freeglut_static
	imgui
	${ADDITIONAL_LIBRARIES}
	${OPENGL_LIBRARIES}
)
