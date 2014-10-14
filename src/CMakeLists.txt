set(qu3e_broadphase_srcs
	broadphase/q3BroadPhase.cpp
	broadphase/q3DynamicAABBTree.cpp
)

set(qu3e_broadphase_hdrs
	broadphase/q3BroadPhase.h
	broadphase/q3DynamicAABBTree.h
	broadphase/q3DynamicAABBTree.inl
)

set(qu3e_collision_srcs
	collision/q3Box.cpp
	collision/q3Collide.cpp
)

set(qu3e_collision_hdrs
	collision/q3Box.h
	collision/q3Box.inl
	collision/q3Collide.h
)

set(qu3e_common_srcs
	common/q3Geometry.cpp
	common/q3Memory.cpp
)

set(qu3e_common_hdrs
	common/q3Geometry.h
	common/q3Geometry.inl
	common/q3Memory.h
	common/q3Settings.h
	common/q3Types.h
)

set(qu3e_debug_hdrs
	debug/q3Render.h
)

set(qu3e_dynamics_srcs
	dynamics/q3Body.cpp
	dynamics/q3Contact.cpp
	dynamics/q3ContactManager.cpp
	dynamics/q3ContactSolver.cpp
	dynamics/q3Island.cpp
)

set(qu3e_dynamics_hdrs
	dynamics/q3Body.h
	dynamics/q3Contact.h
	dynamics/q3ContactManager.h
	dynamics/q3ContactSolver.h
	dynamics/q3Island.h
)

set(qu3e_math_srcs
	math/q3Mat3.cpp
	math/q3Quaternion.cpp
	math/q3Vec3.cpp
)

set(qu3e_math_hdrs
	math/q3Mat3.h
	math/q3Mat3.inl
	math/q3Math.h
	math/q3Math.inl
	math/q3Quaternion.h
	math/q3Transform.h
	math/q3Transform.inl
	math/q3Vec3.h
	math/q3Vec3.inl
)

set(qu3e_scene_srcs
	scene/q3Scene.cpp
)

set(qu3e_scene_hdrs
	scene/q3Scene.h
)

set(qu3e_hdr
	q3.h
)

if(qu3e_build_shared)
	add_library(qu3e_shared SHARED
		${qu3e_broadphase_srcs}
		${qu3e_broadphase_hdrs}
		${qu3e_collision_srcs}
		${qu3e_collision_hdrs}
		${qu3e_common_srcs}
		${qu3e_common_hdrs}
		${qu3e_debug_hdrs}
		${qu3e_dynamics_srcs}
		${qu3e_dynamics_hdrs}
		${qu3e_math_srcs}
		${qu3e_math_hdrs}
		${qu3e_scene_srcs}
		${qu3e_scene_hdrs}
		${qu3e_hdr}
	)
	
	set_target_properties(qu3e_shared PROPERTIES
		OUTPUT_NAME "qu3e"
		CLEAN_DIRECT_OUTPUT 1
		VERSION ${qu3e_version}
	)
endif()

if(qu3e_build_static)
	add_library(qu3e STATIC
		${qu3e_broadphase_srcs}
		${qu3e_broadphase_hdrs}
		${qu3e_collision_srcs}
		${qu3e_collision_hdrs}
		${qu3e_common_srcs}
		${qu3e_common_hdrs}
		${qu3e_debug_hdrs}
		${qu3e_dynamics_srcs}
		${qu3e_dynamics_hdrs}
		${qu3e_math_srcs}
		${qu3e_math_hdrs}
		${qu3e_scene_srcs}
		${qu3e_scene_hdrs}
		${qu3e_hdr}
	)
	
	set_target_properties(qu3e PROPERTIES
		OUTPUT_NAME "qu3e"
		CLEAN_DIRECT_OUTPUT 1
		VERSION ${qu3e_version}
	)
endif()

source_group(broadphase FILES ${qu3e_broadphase_srcs} ${qu3e_broadphase_hdrs})
source_group(collision FILES ${qu3e_collision_srcs} ${qu3e_collision_hdrs})
source_group(common FILES ${qu3e_common_srcs} ${qu3e_common_hdrs})
source_group(debug FILES ${qu3e_debug_hdrs})
source_group(dynamics FILES ${qu3e_dynamics_srcs} ${qu3e_dynamics_hdrs})
source_group(math FILES ${qu3e_math_srcs} ${qu3e_math_hdrs})
source_group(scene FILES ${qu3e_scene_srcs} ${qu3e_scene_hdrs})
source_group(Include FILES ${qu3e_hdr})
