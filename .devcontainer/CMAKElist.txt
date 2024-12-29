cmake_minimum_required(VERSION 3.10)

# Project name and version
project(PMLL_Blockchain VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
add_executable(pmll_blockchain
    src/main.cpp
    src/blockchain.cpp
    src/transaction.cpp
    # Add more source files as needed
)

# Include directories
target_include_directories(pmll_blockchain PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Link libraries if required
# target_link_libraries(pmll_blockchain PRIVATE some_library)

# Ubuntu-specific configuration
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring for Ubuntu 2025 updates")
    # Ensure required packages are available
    find_package(Threads REQUIRED)
    target_link_libraries(pmll_blockchain PRIVATE Threads::Threads)

    # Additional settings for GCC compatibility
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    endif()
endif()

# Windows-specific configuration
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring for Windows 2025 updates")
    # Ensure required libraries and settings
    target_compile_definitions(pmll_blockchain PRIVATE _WIN32_WINNT=0x0A00) # Windows 10 or later
    target_link_libraries(pmll_blockchain PRIVATE ws2_32)

    # Additional settings for MSVC compatibility
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
endif()

# Devcontainer integration (commented as this is unrelated to CMake directly)
# "name": "C++ with PMLL_Blockchain",
# "build": {
# 	"dockerfile": "Dockerfile",
# 	"context": ".",
# 	"args": {
# 		"INSTALL_DEPENDENCIES": "true"
# 	}
# },
# "features": {
# 	"ghcr.io/elanhasson/devcontainer-features/dotnet-aspire-daily:1": {},
# 	"ghcr.io/nikiforovall/devcontainer-features/dotnet-aspire:1": {},
# 	"ghcr.io/nikiforovall/devcontainer-features/dotnet-csharpier:1": {}
# },
# "customizations": {
# 	"vscode": {
# 		"extensions": [
# 			"ms-vscode.cmake-tools",
# 			"ms-dotnettools.csharp",
# 			"ms-vscode.cpptools",
# 			"ms-python.python",
# 			"ms-vscode.powershell"
# 		]
# 	}
# },
# "forwardPorts": [55787],
# "postCreateCommand": "gcc --version && cmake . && make && ./run_tests",
# "remoteUser": "root",
# "mounts": [
# 	"source=/var/run/docker.sock,target=/var/run/docker.sock,type=bind"
# ],
# "workspaceMount": "source=${localWorkspaceFolder},target=/workspace,type=bind,consistency=cached",
# "workspaceFolder": "/workspace/pmll_blockchain"
