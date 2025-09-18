# Backend CMake
file(GLOB_RECURSE BACKEND_SOURCES CONFIGURE_DEPENDS "*.cpp")
add_library(backend STATIC ${BACKEND_SOURCES})

target_include_directories(backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(MYSQLCPPCONN REQUIRED mysqlcppconn)

# Include directories
target_include_directories(backend PUBLIC ${MYSQLCPPCONN_INCLUDE_DIRS})

# Link libraries
target_link_libraries(backend PRIVATE 
    pthread 
    ${MYSQLCPPCONN_LIBRARIES}
    mysqlcppconn
    jsoncpp
    crow
)

# Add executable
add_executable(schedule_server main.cpp)
target_link_libraries(schedule_server PRIVATE backend)

# Copy frontend files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/frontend DESTINATION ${CMAKE_BINARY_DIR})
