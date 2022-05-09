
include(${NAP_ROOT}/cmake/dist_shared_crossplatform.cmake)

set(OpenCV_DIR "C:/dev/opencv/opencv/build")
set(depthai_DIR "C:/Users/pierr/Documents/DEV/DEPTHAI_DEV/build_2")

find_package(OpenCV REQUIRED)
find_package(depthai CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} depthai::opencv ${OpenCV_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC ${depthai_DIR}/include)


if(WIN32)
    set(CUSTOM_OPENCV_LIBS_DEBUG ${OpenCV_DIR}/x64/vc15/lib/opencv_world455d.lib)
    set(CUSTOM_OPENCV_LIBS_RELEASE ${OpenCV_DIR}/x64/vc15/lib/opencv_world455.lib)
    set(CUSTOM_OPENCV_LIBS_DIR ${OpenCV_DIR}/x64/vc15/bin)
    set(CUSTOM_OPENCV_DEBUG_DLL ${OpenCV_DIR}/x64/vc15/bin/opencv_world455d.dll)
    set(CUSTOM_OPENCV_RELEASE_DLL ${OpenCV_DIR}/x64/vc15/bin/opencv_world455.dll)
endif()


mark_as_advanced(CUSTOM_OPENCV_LIBS_DIR)
mark_as_advanced(CUSTOM_OPENCV_DEBUG_DLL)
mark_as_advanced(CUSTOM_OPENCV_RELEASE_DLL)



include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(customOpenCV REQUIRED_VARS OpenCV_DIR)


add_library(customOpenCV SHARED IMPORTED)


set_target_properties(customOpenCV PROPERTIES
                      IMPORTED_CONFIGURATIONS "Debug;Release;"
                      IMPORTED_LOCATION_RELEASE ${CUSTOM_OPENCV_RELEASE_DLL}
                      IMPORTED_LOCATION_DEBUG ${CUSTOM_OPENCV_DEBUG_DLL}
                      )

if(WIN32)
    set_target_properties(customOpenCV PROPERTIES
                        IMPORTED_IMPLIB_RELEASE ${CUSTOM_OPENCV_LIBS_RELEASE}
                        IMPORTED_IMPLIB_DEBUG ${CUSTOM_OPENCV_LIBS_DEBUG}
                        )
endif()


if(WIN32)
    # Copy over DLLs post-build on Windows
    file(GLOB OpenCVDLL ${OpenCV_DIR}/x64/vc15/bin/*.dll)
    set(DLLCOPY_PATH_SUFFIX "")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} 
        -E copy ${CUSTOM_OPENCV_DEBUG_DLL} 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DLLCOPY_PATH_SUFFIX}
    )

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} 
        -E copy ${CUSTOM_OPENCV_RELEASE_DLL} 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DLLCOPY_PATH_SUFFIX}
    )
endif()