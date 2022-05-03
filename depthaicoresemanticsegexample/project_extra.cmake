
include(${NAP_ROOT}/user_modules/mod_depthaicore/module_extra.cmake)

if(WIN32)
    message(STATUS "included the extra piece of cmake to copy OpenCV dll in the example")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CUSTOM_OPENCV_DEBUG_DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DLLCOPY_PATH_SUFFIX}
    )
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CUSTOM_OPENCV_RELEASE_DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DLLCOPY_PATH_SUFFIX}
    )
endif()