cmake_minimum_required (VERSION 2.6)

add_executable( display_tags display_tags.cc )
target_link_libraries(display_tags fidstr ${FiducialStream_LIBRARIES})

install(TARGETS display_tags
    RUNTIME DESTINATION bin
)
