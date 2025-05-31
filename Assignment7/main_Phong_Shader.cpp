#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <fstream>
#include <iostream>
#include <string>

GLuint compileShader(const char* path, GLenum type)
{
    std::ifstream ifs(path);
    std::string src((std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
    const char* csrc = src.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);

    int success; glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512]; glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "[Shader] " << path << "\n" << log << std::endl;
        exit(EXIT_FAILURE);
    }
    return shader;
}
GLuint linkProgram(GLuint vs, GLuint fs)
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    int success; glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512]; glGetProgramInfoLog(prog, 512, nullptr, log);
        std::cerr << "[Link]\n" << log << std::endl;
        exit(EXIT_FAILURE);
    }
    glDeleteShader(vs); glDeleteShader(fs);
    return prog;
}

void createSphere(int segW, int segH,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices)
{
    const float PI = 3.14159265358979323846f;

    for (int y = 0; y <= segH; ++y) {
        float v = float(y) / segH;
        float theta = v * PI;
        for (int x = 0; x <= segW; ++x) {
            float u = float(x) / segW;
            float phi = u * 2.0f * PI;

            glm::vec3 p(std::sin(theta) * std::cos(phi),
                std::cos(theta),
                -std::sin(theta) * std::sin(phi));
            vertices.insert(vertices.end(), { p.x, p.y, p.z });
            vertices.insert(vertices.end(), { p.x, p.y, p.z });
        }
    }
    for (int y = 0; y < segH; ++y)
        for (int x = 0; x < segW; ++x) {
            unsigned int i0 = (segW + 1u) * y + x;
            unsigned int i1 = (segW + 1u) * (y + 1) + x;
            unsigned int i2 = (segW + 1u) * (y + 1) + (x + 1);
            unsigned int i3 = (segW + 1u) * y + (x + 1);

            indices.insert(indices.end(), { i0, i1, i3 });
            indices.insert(indices.end(), { i3, i1, i2 });
        }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    GLFWwindow* win = glfwCreateWindow(512, 512, "HW7 – Phong Shader", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, 512, 512);
    GLuint vs = compileShader("Phong.vert", GL_VERTEX_SHADER);
    GLuint fs = compileShader("Phong.frag", GL_FRAGMENT_SHADER);
    GLuint prog = linkProgram(vs, fs);

    std::vector<float> verts;
    std::vector<unsigned int> idx;
    createSphere(32, 16, verts, idx);

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        idx.size() * sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glm::vec3 eye(0.0f, 0.0f, 0.0f);
    glm::vec3 lightPos(-4.0f, 4.0f, -3.0f);

    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
        glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(prog);

        glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f),
            glm::vec3(0.0f, 0.0f, -7.0f)),
            glm::vec3(2.0f));
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::frustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 1000.0f);

        glUniformMatrix4fv(glGetUniformLocation(prog, "uModel"), 1, GL_FALSE,
            glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(prog, "uView"), 1, GL_FALSE,
            glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(prog, "uProjection"), 1, GL_FALSE,
            glm::value_ptr(proj));

        glUniform3fv(glGetUniformLocation(prog, "uLightPos"), 1,
            glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(prog, "uEyePos"), 1,
            glm::value_ptr(eye));

        glUniform3f(glGetUniformLocation(prog, "uKa"), 0.0f, 1.0f, 0.0f);
        glUniform3f(glGetUniformLocation(prog, "uKd"), 0.0f, 0.5f, 0.0f);
        glUniform3f(glGetUniformLocation(prog, "uKs"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(prog, "uShininess"), 32.0f);
        glUniform1f(glGetUniformLocation(prog, "uIa"), 0.2f);
        glUniform3f(glGetUniformLocation(prog, "uLightColor"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(prog, "uGamma"), 2.2f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, idx.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(win);
    }

    glDeleteProgram(prog);
    glDeleteBuffers(1, &VBO); glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
