# Repository Guidelines

## Project Structure & Module Organization

- `modules/common/` contains logging, YAML configuration, timers, queues, and thread utilities; `modules/ipc/` implements System V message queues. Public headers live in `include/<module>/`, implementations in `src/`.
- `modules/common/tests/` holds unit tests. `app/timer_app/` and `app/ipc_app/` provide example executables.
- `3rdparty/` contains dependency integration; `cmake/` contains build, analysis, and packaging helpers. Runtime settings live in `config/params.yaml`, service definitions in `serve/`, plotting tools in `tools/`, and report templates in `xslt/`.
- Keep generated binaries, libraries, and installation output in `build/` and `install/`.

## Build, Test, and Development Commands

Use Linux with a C++17 compiler, CMake, Make, Eigen3, Python3, Cppcheck, and `xsltproc`. Prepare spdlog headers and yaml-cpp headers/shared libraries under their `3rdparty/` directories; dependency versions are listed in `3rdparty/README.md`.

- `make build`: configure a Release build and compile into `build/bin/` and `build/lib/`.
- `make build build_type=Debug`: compile a Debug build.
- `make test`: enable `ENABLE_TEST`, build, and run CTest with failure output.
- `make check`: run Cppcheck and generate `build/cppcheck_report.html`.
- `./build/bin/timer_node`: run the timer example.
- `make install` / `make pack`: stage files in `install/` or create a Debian package.

Always build outside the source directory; in-source CMake builds are rejected.

## Coding Style & Naming Conventions

Follow the Google-style formatting used in module code: two-space indentation and braces on the declaration line. Use `.hpp` headers, `.cpp` implementations, `snake_case` filenames, `PascalCase` classes, and trailing underscores for private members. Preserve surrounding API naming conventions, including existing `GetInstance`/`GetParam` methods. No formatter configuration is checked in; use `make check` for static analysis.

## Testing Guidelines

Tests use doctest through CTest. Name files `test_<feature>.cpp` and register targets with `add_common_test(...)` in `modules/common/tests/CMakeLists.txt`. Cover normal behavior, failure paths, and concurrency where relevant. Run `make test` before submitting; rerun one registered test from `build/` with `ctest -R test_safe_queue --output-on-failure`. No numeric coverage threshold is configured.

## Commit & Pull Request Guidelines

Use `<type>: <description>`, matching history and `.githook.sh`; common types include `feat`, `fix`, `docs`, `refactor`, `test`, `chore`, and `perf`. Example: `fix: handle stopped queue`. Install the optional commit-message hook with `bash .githook.sh`.

Keep pull requests focused. Describe the behavior change, link related issues, report test and analysis results, and identify configuration or packaging impacts.
