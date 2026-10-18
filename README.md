

# C++ 3D Ray Tracer

A CPU-based offline 3D Ray Tracer built in modern C++ using Microsoft Visual Studio. The project focuses on efficient multithreaded rendering, custom BRDF lighting models, acceleration structures via AABB slab testing, and an interactive viewport camera system.

---

<img width="627" height="458" alt="Screenshot 2026-10-18 202301" src="https://github.com/user-attachments/assets/3f4236a1-eb88-4e34-b7cb-b8b97ee3bdda" />

<img width="640" height="480" alt="2026-10-1820-36-56-ezgif com-video-to-gif-converter" src="https://github.com/user-attachments/assets/bf8fb451-a879-4f87-a7f9-33b548946da0" />



## 📸 Core Features & Custom Implementations

* **Multi-Threaded Pixel Pipeline**:
  * Parallelizes render passes across CPU threads using C++ concurrency (`std::thread`).
  

* **Lighting Models & BRDFs (`BRDFs.h`)**:
  * **Lambertian Diffuse**: Simulates uniform light scattering across matte surfaces.
  * **Phong / Blinn-Phong Specular**: Glossy highlight reflections with configurable roughness/shininess exponents.
  * **Interactive Shading Modes**: Allows runtime toggling between combined lighting, observed area, radiance, or BRDF components.

* **Acceleration Structures**:
  * **Axis-Aligned Bounding Box (AABB) Slab Test**: Implements ray-AABB slab intersection tests (`tmin = max(...)`, `tmax = min(...)`) to quickly prune objects outside ray trajectories before executing complex surface hit checks.

* **Interactive Camera System (`Camera.h`)**:
  * Real-time view matrix transformations supporting Field of View (FOV), target tracking, and dynamic mouse/keyboard movement.

---

## 🎮 Interactive Controls

### Mouse Controls

| Input | Action |
| :--- | :--- |
| **RMB + Mouse Move X** | Rotate Camera Yaw |
| **RMB + Mouse Move Y** | Rotate Camera Pitch |
| **LMB + Mouse Move Y** | Move Camera (Local) Forward / Backward |
| **LMB + Mouse Move X** | Rotate Camera Yaw |
| **LMB + RMB + Mouse Move X/Y** | Pan View Plane (Up / Down / Left / Right) |

### Keyboard Controls

| Key | Action |
| :--- | :--- |
| **W** or **Up Arrow** | Move Camera (Local) Forward |
| **S** or **Down Arrow** | Move Camera (Local) Backward |
| **A** or **Left Arrow** | Move Camera (Local) Left |
| **D** or **Right Arrow** | Move Camera (Local) Right |
| **F2** | Toggle Shadows ON / OFF |
| **F3** | Toggle Light-Equation Mode 

---

## ⚙️ Performance Optimization Techniques

1. **AABB Slab Intersection Test**: Uses min/max ray-plane slab logic to reject non-intersecting primitives early, preventing expensive hit routines.
2. **Multi-Threaded Work Chunking**: Distributes render tiles/rows across available CPU logical threads to avoid single-thread bottlenecks.
3. **No Heap Allocations in Ray Loop**: Allocates all ray intersection data structures on the stack or pre-allocates buffer memory upfront to eliminate mutex lock overhead during parallel rendering.
4. **Const-Reference & Inlining**: Core lighting evaluation functions, ray checks, and color utilities pass parameters via `const T&` and leverage compiler inline hints to eliminate stack frame overhead.

---

## 🛠️ Repository Structure

```text
├── Renderer.h/.cpp     # Core multithreaded ray-tracing engine & pixel loops
├── Scene.h/.cpp        # Scene graph, light sources, AABB, and object intersections
├── Camera.h            # Interactive camera positioning, FOV, and ray casting
├── BRDFs.h             # Custom Lambertian, Phong, and Blinn-Phong BRDF implementations
├── Material.h          # Material shading properties (reflectivity, roughness, color)
├── Main.cpp            # Entry point, event loop, and user input handling
└── [Boilerplate / Math / Utilities]
