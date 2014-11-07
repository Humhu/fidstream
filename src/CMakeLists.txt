cmake_minimum_required(VERSION 2.8)

set(SRCS
	AprilTagFiducial.cc
	AprilTagPhysicalLayer.cc
	DataLinkLayer.cc
	Fiducial.cc
	PhysicalLayer.cc
	SerialPacketizer.cc
	SynchronizationPrimitives.cc )

add_library(fidstr SHARED ${SRCS})

file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/include/fidstr/*.h"
    "${PROJECT_SOURCE_DIR}/include/fidstr/*.hpp")

set_target_properties(fidstr PROPERTIES
    OUTPUT_NAME fidstr
    PUBLIC_HEADER "${HEADERS}")

install(TARGETS fidstr
    EXPORT FiducialStreamTargets
    LIBRARY DESTINATION "${LIBRARY_INSTALL_DIR}" COMPONENT shlib
    PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}"
    COMPONENT dev)

target_link_libraries(fidstr ${FiducialStream_LIBRARIES})
