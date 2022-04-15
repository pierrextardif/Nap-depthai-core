

set(OpenCV_DIR "C:\\dev\\opencv\\opencv\\build")
find_package(OpenCV REQUIRED)
set(depthai_DIR "C:\\Users\\pierr\\Documents\\DEV\\DEPTHAI_DEV\\build_2")
find_package(depthai CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} depthai::opencv ${OpenCV_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC ${depthai_DIR}/include)

if(WIN32)
    # Copy over DLLs post-build on Windows
    file(GLOB OpenCVDLL ${OpenCV_DIR}/x64/vc15/bin/*.dll)

    set(DLLCOPY_PATH_SUFFIX "")
    foreach (SINGLE_DLL ${OpenCVDLL})
	    add_custom_command(
		    TARGET ${PROJECT_NAME}
		    POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${SINGLE_DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DLLCOPY_PATH_SUFFIX}
		)
	endforeach()
elseif(APPLE)
    # Add FFmpeg RPATH to built app
    macos_add_rpath_to_module_post_build(${PROJECT_NAME} $<TARGET_FILE:${PROJECT_NAME}> ${THIRDPARTY_DIR}/FFmpeg/lib) 

    # Install FFmpeg into packaged app
    install(DIRECTORY ${THIRDPARTY_DIR}/FFmpeg/lib/ DESTINATION lib)
elseif(UNIX)
    # Install FFmpeg into packaged app
    install(DIRECTORY ${THIRDPARTY_DIR}/FFmpeg/lib/ DESTINATION lib)
endif()
