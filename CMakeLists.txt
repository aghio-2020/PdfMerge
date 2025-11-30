cmake_minimum_required(VERSION 3.10)

project(PdfMerge)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set dependencies directories based on platform
set(DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies")
if(WIN32)
    set(PLATFORM_DIR "${DEPENDENCIES_DIR}/windows")
    set(PDFIO_LIB_NAME "pdfio1")
else()
    set(PLATFORM_DIR "${DEPENDENCIES_DIR}/linux")
    set(PDFIO_LIB_NAME "pdfio")
endif()

set(PDFIO_INCLUDE_DIR "${PLATFORM_DIR}/include")
set(PDFIO_LIB_DIR "${PLATFORM_DIR}/lib")

# Include directories
include_directories(${PDFIO_INCLUDE_DIR})

# Source files
set(SOURCES main.cpp)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# Create executable
add_executable(pdfMerge ${SOURCES})

# Link directories
link_directories(${PDFIO_LIB_DIR})

# Link libraries
target_link_libraries(pdfMerge PRIVATE ${PDFIO_LIB_NAME} z)

# Set RPATH for runtime library search
set_target_properties(pdfMerge PROPERTIES
    BUILD_RPATH ${PDFIO_LIB_DIR}
    INSTALL_RPATH ${PDFIO_LIB_DIR}
)

