# Use an official C++ compiler image
FROM ubuntu:22.04

# Set non-interactive mode for apt
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools, compilers, and CMake
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /app

# Copy the source code into the container
COPY . /app

# Create a build directory and compile the project
RUN mkdir -p build && cd build && \
    cmake .. && \
    make

# Default command when container runs
CMD ["./build/my_project"]