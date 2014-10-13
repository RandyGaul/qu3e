set(SRCROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(qu3e_SOURCES 
    ${SRCROOT}/broadphase/q3BroadPhase.cpp
    ${SRCROOT}/broadphase/q3DynamicAABBTree.cpp

    ${SRCROOT}/collision/q3Box.cpp
    ${SRCROOT}/collision/q3Collide.cpp

    ${SRCROOT}/common/q3Geometry.cpp
    ${SRCROOT}/common/q3Memory.cpp

    ${SRCROOT}/dynamics/q3Body.cpp
    ${SRCROOT}/dynamics/q3Contact.cpp
    ${SRCROOT}/dynamics/q3ContactManager.cpp
    ${SRCROOT}/dynamics/q3ContactSolver.cpp
    ${SRCROOT}/dynamics/q3Island.cpp

    ${SRCROOT}/math/q3Mat3.cpp
    ${SRCROOT}/math/q3Quaternion.cpp
    ${SRCROOT}/math/q3Vec3.cpp

    ${SRCROOT}/scene/q3Scene.cpp
)
add_library(qu3e ${qu3e_SOURCES})


install(TARGETS qu3e
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ../include DESTINATION ${CMAKE_INSTALL_PREFIX}
          FILES_MATCHING PATTERN "*.h" PATTERN ".inl")

