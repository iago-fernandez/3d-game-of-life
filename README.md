# Game of Life

A modular and modern implementation of **Conway’s Game of Life** written in **C++17** using **OpenGL 3.3**, **GLFW**, **GLAD**, **GLM**, and **ImGui**.  

The project renders the simulation in real time through synchronized **2D grid** and **3D torus** views, allowing users to explore both the **cellular automaton dynamics** and its **topological interpretation**.

Beyond its visual appeal, the project also serves as:
- A minimal **computational model of population growth and decay**, based on local interaction rules.  
- A **didactic visualization of toroidal geometry**, showing how a 2D grid can be seamlessly wrapped into a 3D surface.  
- An example of **modern C++ and OpenGL architecture**, with modular systems for logic, rendering, and UI integration.  
- A compact and extensible base for experimentation with **GPU simulations**, **shader-based visualizations**, or **scientific cellular automata** (see *CONTRIBUTE.md* for more info).

The codebase emphasizes clarity, maintainability, and clean design—ideal for studying or extending.

---

## Features

- Real-time simulation with adjustable fixed-step timing  
- Dual visualization modes:  
  - 2D grid with interactive cell editing  
  - 3D torus view with an orbit camera  
- Fully modular architecture:  
  - `core/` – simulation logic  
  - `render/` – 2D and 3D rendering  
  - `ui/` – ImGui toolbar  
  - `app/` – lifecycle and systems integration  
- Modern OpenGL pipeline (VAO, VBO, EBO, GLSL shaders)  
- User interface controls for:  
  - Play / Pause / Step / Clear  
  - Grid size (rows and columns)  
  - Simulation speed  

---

## Requirements

- Windows 10 / 11  
- CMake ≥ 3.23  
- Visual Studio 2022 (MSVC) or compatible compiler  
- Ninja (build system used by presets)  
- vcpkg (dependency manager)

> The project depends on: **glfw3**, **glad**, **glm**, and **imgui**.

---

## Interface

The window is divided into two synchronized viewports and a compact toolbar:

- **Left — 2D grid**: interactive editor (toggle cells with left click), hover highlight, and grid overlay.
- **Right — 3D torus**: orbit camera (left click + drag) and zoom (scroll), showing the same state wrapped on a toroidal surface.
- **Toolbar (bottom-right)**: Play/Pause, Step, Clear, **Speed** slider, and **Rows / Columns** (applied on *Enter* or when the field loses focus).

![App overview](assets/screenshots/appView.png)

---

## Controls

| Context | Action | Description |
|----------|---------|-------------|
| **2D View** (left pane) | Left Click | Toggle cell |
| **3D View** (right pane) | Left Click + Drag | Orbit camera (yaw/pitch) |
| | Scroll | Zoom in/out |
| **UI (Toolbar)** | Play / Pause / Step / Clear | Simulation control |
| | Rows / Columns | Apply on Enter or focus loss |
| | Speed | Adjust steps per second |
