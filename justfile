set shell := ["bash", "-euo", "pipefail", "-c"]

build_dir := env_var_or_default("BUILD_DIR", "build")
build_type := env_var_or_default("BUILD_TYPE", "Debug")
jobs := env_var_or_default("JOBS", "10")
step_budget := env_var_or_default("STELLA_GC_STEP_BUDGET", "1")

default:
    @just --list

# Configure cmake
configure:
    cmake \
        -S . \
        -B {{build_dir}} \
        -DCMAKE_BUILD_TYPE={{build_type}} \
        -DBUILD_TESTING=ON

# Build project, examples, and tests
build: configure
    cmake \
        --build {{build_dir}} \
        --parallel {{jobs}}

# Run tests
test: build
    STELLA_GC_STEP_BUDGET={{step_budget}} \
        ctest \
        --test-dir {{build_dir}} \
        --output-on-failure

# remove build dir with all artifacts
clean:
    rm -rf {{build_dir}}

# fresh rebuild
rebuild: clean build