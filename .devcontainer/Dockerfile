# Use the base image for C++ development
FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

# Set the CMake version to reinstall if needed
ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# Copy the script to reinstall CMake from source
COPY ./reinstall-cmake.sh /tmp/

# Run the script to reinstall CMake if specified
RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Install additional vcpkg ports
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Install additional packages
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

# Set environment variables for your project
ENV VCPKG_INSTALLATION_ROOT /usr/local/vcpkg
ENV JAVA_HOME_17_X64 /usr/lib/jvm/java-17-openjdk-amd64
# Add more environment variables as needed

# Install necessary software
# Note: Since this is a Debian-based image, we'll use apt for package management
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
       python3-pip \
       nodejs \
       npm \
       openjdk-17-jdk \
       git \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Python setup
RUN python3 -m pip install --upgrade pip

# Node.js setup
RUN npm install -g yarn

# Install vcpkg if not already present (assuming it's not in the base image)
RUN git clone https://github.com/microsoft/vcpkg.git $VCPKG_INSTALLATION_ROOT \
    && cd $VCPKG_INSTALLATION_ROOT \
    && ./bootstrap-vcpkg.sh

# Copy project files into the container
COPY . /workspace

# Set the working directory in the container
WORKDIR /workspace

# Default command when container starts
CMD ["bash"]
