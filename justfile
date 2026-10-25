set shell := ["bash", "-euo", "pipefail", "-c"]

build_dir := env_var_or_default("BUILD_DIR", "build")
sanitize_build_dir := env_var_or_default("SANITIZE_BUILD_DIR", "build_sanitize")
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
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DBUILD_TESTING=ON

# Configure cmake
sanitize-configure:
    cmake \
        -S . \
        -B {{sanitize_build_dir}} \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DBUILD_TESTING=ON \
        -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
        -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
        -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"

# Build project, examples, and tests
build: configure
    cmake \
        --build {{build_dir}} \
        --parallel {{jobs}}

sanitize-build: sanitize-configure
    cmake \
        --build {{sanitize_build_dir}} \
        --parallel {{jobs}}

# Run tests
test: build
    STELLA_GC_STEP_BUDGET={{step_budget}} \
        ctest \
        --test-dir {{build_dir}} \
        --output-on-failure

sanitize-test: sanitize-build
    STELLA_GC_STEP_BUDGET={{step_budget}} \
        ctest \
        --test-dir {{sanitize_build_dir}} \
        --output-on-failure

# remove build dir with all artifacts
clean:
    rm -rf {{build_dir}}

# fresh rebuild
rebuild: clean build