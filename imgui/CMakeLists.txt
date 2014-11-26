set(imgui_srcs
	imgui.cpp
)

set(imgui_hdrs
	imgui.h
	imconfig.h
	stb_textedit.h
)

include_directories (
	${OPENGL_INCLUDE_DIR}
	.
)

add_library(imgui
	${imgui_srcs}
	${imgui_hdrs}
)
