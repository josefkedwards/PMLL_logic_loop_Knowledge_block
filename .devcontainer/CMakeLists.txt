cmake_minimum_required(VERSION 3.10)

# -----------------------------------------------------------------------------
# Project name and version
# -----------------------------------------------------------------------------
project(PMLL_Blockchain VERSION 1.0)

# -----------------------------------------------------------------------------
# C++ standard requirements
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Options (optional)
# -----------------------------------------------------------------------------
option(PMLL_BUILD_TESTS "Build tests" ON)         # Toggle test builds
option(PMLL_BUILD_EXAMPLES "Build examples" OFF)  # Toggle example builds

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
set(SRC_FILES
    src/main.cpp
    src/blockchain.cpp
    src/transaction.cpp
    # Add more source files as needed...
)

# -----------------------------------------------------------------------------
# Include directories
# -----------------------------------------------------------------------------
# If you keep all headers in an `include/` folder, for example:
# include_directories(${CMAKE_SOURCE_DIR}/include)

# -----------------------------------------------------------------------------
# Create the executable
# -----------------------------------------------------------------------------
add_executable(pmll_blockchain ${SRC_FILES})

# -----------------------------------------------------------------------------
# Link libraries (if needed)
# -----------------------------------------------------------------------------
# For example, if you use OpenSSL for cryptography:
# find_package(OpenSSL REQUIRED)
# if(OPENSSL_FOUND)
#     target_include_directories(pmll_blockchain PRIVATE ${OPENSSL_INCLUDE_DIR})
#     target_link_libraries(pmll_blockchain PRIVATE OpenSSL::SSL OpenSSL::Crypto)
# endif()

# -----------------------------------------------------------------------------
# Add compiler warnings (optional)
# -----------------------------------------------------------------------------
if(MSVC)
    target_compile_options(pmll_blockchain PRIVATE /W4 /permissive-)
else()
    target_compile_options(pmll_blockchain PRIVATE -Wall -Wextra -pedantic)
endif()

# -----------------------------------------------------------------------------
# Tests (optional)
# -----------------------------------------------------------------------------
if(PMLL_BUILD_TESTS)
    enable_testing()
    # If using Google Test, for instance:
    # add_subdirectory(tests)
endif()

# -----------------------------------------------------------------------------
# Install targets (optional)
# -----------------------------------------------------------------------------
# install(TARGETS pmll_blockchain
#     RUNTIME DESTINATION bin
# )

# -----------------------------------------------------------------------------
# Summary
# -----------------------------------------------------------------------------
message(STATUS "--------------------------------------------------")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Tests: ${PMLL_BUILD_TESTS}")
message(STATUS "Build Examples: ${PMLL_BUILD_EXAMPLES}")
message(STATUS "--------------------------------------------------")

