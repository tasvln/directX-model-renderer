# DirectX 12 Model Renderer

## Overview
This project builds upon the base DirectX 12 graphics pipeline and introduces **model rendering**, **advanced lighting**, and **material systems**. It uses **Assimp** for model importing and supports **Phong** and **Blinn-Phong** lighting, **shadow mapping**, **normal mapping**, and **parallax mapping**.  
The system also includes an **enhanced camera**, a **custom logging system**, and efficient resource management for real-time 3D rendering.


<img width="1435" height="722" alt="Screenshot 2025-10-08 135610" src="https://github.com/user-attachments/assets/6abedc4f-6ca0-4540-84fb-bbfeac6ed8b5" />

https://github.com/user-attachments/assets/654a5db4-e9b5-4220-ad5a-4494431a18f5

<img width="1440" height="725" alt="Screenshot 2025-10-08 135931" src="https://github.com/user-attachments/assets/262e4e3f-4d47-4745-b85d-fcddafdfdaad" />

---

## Features
- Full DirectX 12 rendering pipeline with modular design  
- Model loading via **Assimp** (OBJ, FBX, GLTF supported)  
- Mesh abstraction for vertex/index buffers  
- Material system supporting multiple texture types  
- **Directional, Point, and Spot Lights**  
- **Phong** and **Blinn-Phong** shading models  
- **Shadow Mapping** with depth texture rendering  
- **Normal and Parallax Mapping** for surface detail  
- Enhanced camera system (orbit + free movement)  
- Custom logger for categorized runtime messages (info, warn, error)  
- FPS counter displayed in the window title  

---

## Concepts Demonstrated
- Assimp-based model importing  
- Vertex attributes (position, normal, tangent, UVs)  
- Texture binding with descriptor heaps  
- Multiple lighting models and light types  
- Shadow depth rendering and projection  
- Normal and parallax mapping in tangent space  
- Material system with configurable maps and parameters  
- Camera transformations and free-look movement  
- Command list recording and synchronization  
- Frame timing and performance tracking  

---

## Requirements
- **Language:** C++17 or later  
- **Graphics API:** DirectX 12  
- **IDE:** Visual Studio Code / Visual Studio 2022  
- **SDKs/Libraries:**  
  (Check CMake file for all libraries)

---

## How to Build & Run
Clone the repository:
```bash
git clone https://github.com/yourusername/directx12-model-renderer.git
cd directx12-model-renderer
```

Build shaders:
```bash
cd assets/shaders
bash compile.sh
```

Build the demo (from the root folder):
```bash
bash build.sh
```

The application window will open showing a 3D model rendered with lighting, textures, and shadows.
(you can always change the model being rendered!)

### Notes
- Ensure **Assimp** is installed or properly linked.  
- Verify that `dxc` (or `fxc`) is installed for shader compilation.  
- Run with normal privileges unless elevated access is required for your environment.

---

## Shaders
Place shader HLSL files under `assets/shaders/`. Typical files include:
- **vertex.hlsl** — transforms vertices and computes lighting inputs.  
- **pixel.hlsl** — handles lighting calculations, shadow mapping, and texture sampling.  
- **lighting.hlsl** — helper to handle all lighting calculations. 

---

## Logging and FPS
A **custom logger** provides categorized messages (`INFO`, `WARN`, `ERROR`).  
Logs can be sent to both console and file output.  
An **FPS counter** is calculated in the main render loop and displayed in the window title.

---

## References
- https://github.com/FaberSanZ/DX12-Samples  
- https://github.com/tasvln/Xbox-ATG-Samples  
- https://github.com/assimp/assimp  
- https://learnopengl.com/  

---
