# Contributing

Thank you for your interest in improving the 3D Game of Life Engine. This project focuses on modern C++ practices, strict resource management, and graphics performance. Please review the guidelines below to ensure your contribution aligns with the project's architectural standards.

## Engineering Standards

To maintain stability and portability across graphics drivers and operating systems, contributions must adhere to the following:

* **Modern C++ (C++17):** Use `auto`, structured bindings, and `constexpr` where appropriate. Avoid C-style casts; use `static_cast` or `reinterpret_cast`.
* **Resource Management (RAII):**
    * **Never** use raw `new` or `delete`. Use `std::unique_ptr` or `std::shared_ptr`.
    * GL resources (Textures, VAOs, Shaders) must be wrapped in classes that handle their deletion in the destructor.
* **OpenGL State:**
    * Do not leave OpenGL state dirty. If you bind a buffer or a shader, unbind it (`glBind... (0)`) or restore the previous state before returning control.
    * Use the provided `shaderUtils` namespace for shader compilation to ensure proper error logging.
* **Code Style:**
    * **Indentation:** 4 spaces (no tabs).
    * **Naming:** `camelCase` for functions/variables, `PascalCase` for classes/structs.
    * **Members:** Private member variables must end with an underscore (e.g., `window_`, `width_`).

## Development Workflow

1.  **Fork and Clone** the repository.
2.  **Branching Strategy:**
    * `feat/`: New rendering features or simulation logic.
    * `fix/`: Rendering glitches or logic bugs.
    * `refactor/`: shader optimization or C++ code cleanup.
    * `docs/`: Documentation updates.
3.  **Validation:** Ensure the project compiles cleanly with **Warning Level 4 (`/W4`)** on MSVC or `-Wall -Wextra` on GCC/Clang.

## Commit Guidelines

We use **Conventional Commits** to maintain a clean, semantic history:

* `feat(render): add bloom post-processing effect`
* `fix(sim): correct boundary wrapping logic for negative coordinates`
* `refactor(core): optimize neighbour counting loop`
* `chore: update vcpkg dependencies`

## Pull Request Process

* Provide a clear description of the changes. If visual, **attach a screenshot or GIF**.
* Ensure no new compiler warnings are introduced.
* Squash intermediate commits to keep the history linear before merging.