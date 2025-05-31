Assignment 7 – OpenGL Shader Implementation 

Computer Graphics HW7 – Single‑sphere Phong‐shading pipeline in modern OpenGL

Goals

Build the entire rendering pipeline only with C++ + GLSL (Vertex & Fragment shaders) under OpenGL 3.3 Core Profile.

Re‑use the sphere geometry and transformation setup from HW5, and feed the Phong‑shading parameters from HW6 to the GPU via uniforms so that the final frame perfectly matches the HW6 screenshot.

Absolutely no fixed‑function calls (glMatrixMode, glLight*, glMaterial*, etc.). Everything is programmable.

Project layout

.
├── main_Phong_Shader.cpp   # driver code – sphere generation + uniform uploads
├── Phong.vert              # vertex shader – world transform & MVP
├── Phong.frag              # fragment shader – Phong lighting + gamma
└── README.md               # this document

Tip 🔍 Keep both shader files in the same folder as the executable; the code loads them at runtime.

🚀 Build / Run

Required libraries

Library

Tested Version

Notes

GLFW

 3.3.x

Cross‑platform window / input

GLAD

 OpenGL 3.3 Core Loader

GLM

 0.9.9+

Header‑only math library

Windows (MinGW or MSVC)

mkdir build && cd build
clang++ ../main_Phong_Shader.cpp -o Phong.exe ^
  -I%GLFW_INC% -I%GLAD_INC% -I%GLM_INC% ^
  %GLFW_LIB% %OPENGL32_LIB% %GLAD_LIB% ^
  -std=c++17 -mwindows

Linux / macOS

mkdir build && cd build
g++ ../main_Phong_Shader.cpp -o Phong \
   -lglfw -ldl -lGL -std=c++17

Adjust include / lib paths or use package managers (vcpkg, Homebrew, etc.).

💡 Key implementation points

Topic

Location in code

Details

Sphere mesh

createSphere()

Same tessellation as HW5 (32 × 16), 6‑float stride (pos + normal)

Projection matrix

glm::frustum(-0.1,0.1,-0.1,0.1,0.1,1000)

Directly maps HW5 values (l,r,b,t,n,f) → pixel‑perfect match

Gamma correction

Phong.frag

Uniform uGamma = 2.2; pow(color, 1/γ) then GL_FRAMEBUFFER_SRGB for display precision

Uniform upload

main_Phong_Shader.cpp

Ka, Kd, Ks, shininess, Ia, light pos, eye pos, gamma all sent via glUniform*

No fixed‑function

Everywhere

Pure Core Profile + GLSL shaders

📸 Result

Place the captured screenshot at ./screenshot_hw7.png before committing.

🔧 Runtime keys

F  toggle GL_FRAMEBUFFER_SRGB (compare linear vs. sRGB output)

ESC close application

⚖️ License

This code is provided exclusively for coursework submission. If you redistribute it, please keep the author credit and reference to the assignment.

Author

2025 · Lee RyunJae – Department of Computer Science & Engineering, Metaverse Convergence Graduate School, Konkuk University

