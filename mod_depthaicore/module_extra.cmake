
set(depthai_DIR "C:/Users/pierr/Documents/DEV/DEPTHAI_DEV/depthai-core/build")
find_package(depthai CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} depthai::core)

target_include_directories(${PROJECT_NAME} PUBLIC ${depthai_DIR}/include)