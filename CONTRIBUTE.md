# Contributing

Thank you for your interest in contributing to **Game of Life**. This project aims to demonstrate clean, modular, and modern C++ design using OpenGL and related technologies. Contributions —whether bug fixes, enhancements, or documentation improvements— are always appreciated.

---

## How to Contribute

### Reporting Issues

If you find a bug or a build problem, please open a **GitHub Issue** describing:

* What went wrong and what you expected to happen.
* Steps to reproduce the problem.
* Your system details (OS, compiler, and CMake version).

### Suggesting Improvements

If you’d like to propose a feature or visual improvement, open an **enhancement issue** and describe your idea clearly. Small visual or usability suggestions are also welcome.

### Submitting Code

1. **Fork** the repository and create a new branch:

   ```bash
   git checkout -b feat/your-feature-name
   ```
2. Use **Conventional Commits** (e.g., `fix(core): correct torus update timing`).
3. Follow these short rules:

   * Use **C++17**.
   * Keep comments **in English**.
   * Avoid hardcoded paths and global dependencies.
   * Keep rendering, logic, and UI modules separate.
4. Submit a **Pull Request** summarizing your change.

---

## Code Style

* Follow consistent naming: **PascalCase** for classes, **camelCase** for variables.
* Prefer clear modular code over optimization tricks.
* Use RAII and avoid raw pointers when possible.
* Keep code and comments simple, self-explanatory, and aligned with existing patterns.

---

## Build and Test

Before submitting changes:

* Verify that both **Debug** and **Release** presets build correctly.
* Follow the build instructions in [README.md](README.md).
* Don’t include build outputs or temporary files in commits.

---

## Cross-Platform Contributions

If you extend compatibility to Linux or macOS:

* Add a new preset to `CMakePresets.json`.
* Test locally and describe your setup in the Pull Request.
* Keep file paths **relative** and avoid OS-specific code.

---

## License

By contributing, you agree that your code will be released under the same license as the project. See [LICENSE](LICENSE) for details.
