// grasshopper.cpp - キリギリス単体表示プログラム (蟻とは独立)
#include "mesh_common.hpp"

const int MAX_GH = 10;
int numGH = 1;
float motionSpeed = 1.0f;

// ---------- キリギリス メッシュ生成 ----------
void generateGrasshopperMesh(std::vector<Vertex>& verts, std::vector<uint32_t>& inds) {
    glm::vec3 noPivot(0, 0, 0);

    // スケール係数: 蟻(6mm) に対しキリギリス 60mm = 実効10倍
    constexpr float GH_SCALE = 2.0f;
    size_t vStart = verts.size();

    // ---------- 胴体 ----------
    addSphere(verts, inds, glm::vec3(0, 0.08f, 0.35f),
              glm::vec3(0.13f, 0.11f, 0.16f), 6, 4, 0.0f, -1.0f, 0.0f, noPivot, noPivot);
    addSphere(verts, inds, glm::vec3(0, 0.10f, 0.05f),
              glm::vec3(0.14f, 0.13f, 0.18f), 6, 4, 0.0f, -1.0f, 0.0f, noPivot, noPivot);
    addSphere(verts, inds, glm::vec3(0, 0.10f, -0.35f),
              glm::vec3(0.13f, 0.11f, 0.30f), 6, 4, 0.0f, -1.0f, 0.0f, noPivot, noPivot);

    // ---------- 大顎 ----------
    addTriangle(verts, inds,
        glm::vec3(-0.03f, 0.05f, 0.48f),
        glm::vec3(-0.05f, 0.02f, 0.55f),
        glm::vec3(-0.02f, 0.06f, 0.50f),
        0.0f, -1.0f, 0.0f, noPivot, noPivot);
    addTriangle(verts, inds,
        glm::vec3( 0.03f, 0.05f, 0.48f),
        glm::vec3( 0.05f, 0.02f, 0.55f),
        glm::vec3( 0.02f, 0.06f, 0.50f),
        0.0f, -1.0f, 0.0f, noPivot, noPivot);

    // ---------- 触角 (短く直線) ----------
    {
        glm::vec3 lBase(-0.08f, 0.15f, 0.42f);
        glm::vec3 lTip (-0.15f, 0.28f, 0.55f);
        addPrism(verts, inds, lBase, lTip, 0.012f, 2.0f, 0.0f, 0.0f, lBase, noPivot);
    }
    {
        glm::vec3 rBase( 0.08f, 0.15f, 0.42f);
        glm::vec3 rTip ( 0.15f, 0.28f, 0.55f);
        addPrism(verts, inds, rBase, rTip, 0.012f, 2.0f, 1.0f, 0.0f, rBase, noPivot);
    }

    // ---------- 翅 (partID = 3) ----------
    addTriangle(verts, inds,
        glm::vec3(-0.10f, 0.20f, 0.00f),
        glm::vec3(-0.12f, 0.22f, -0.40f),
        glm::vec3(-0.06f, 0.20f, -0.50f),
        3.0f, 0.0f, 0.0f, noPivot, noPivot);
    addTriangle(verts, inds,
        glm::vec3( 0.10f, 0.20f, 0.00f),
        glm::vec3( 0.12f, 0.22f, -0.40f),
        glm::vec3( 0.06f, 0.20f, -0.50f),
        3.0f, 1.0f, 0.0f, noPivot, noPivot);

    // ---------- 前脚 ----------
    {
        float s = -1.0f;
        glm::vec3 attach(s * 0.10f,  0.10f,  0.15f);
        glm::vec3 knee  (s * 0.25f,  0.05f,  0.20f);
        glm::vec3 foot  (s * 0.30f, -0.15f,  0.22f);
        addPrism(verts, inds, attach, knee, 0.018f, 1.0f, 0.0f, 1.0f, attach, knee);
        addPrism(verts, inds, knee,  foot,  0.014f, 1.0f, 0.0f, 2.0f, attach, knee);
        addTriangle(verts, inds, foot,
            foot + glm::vec3(s * 0.02f, -0.02f, 0.03f),
            foot + glm::vec3(s * 0.01f, -0.01f, 0.01f),
            1.0f, 0.0f, 3.0f, attach, knee);
    }
    {
        float s = +1.0f;
        glm::vec3 attach(s * 0.10f,  0.10f,  0.15f);
        glm::vec3 knee  (s * 0.25f,  0.05f,  0.20f);
        glm::vec3 foot  (s * 0.30f, -0.15f,  0.22f);
        addPrism(verts, inds, attach, knee, 0.018f, 1.0f, 3.0f, 1.0f, attach, knee);
        addPrism(verts, inds, knee,  foot,  0.014f, 1.0f, 3.0f, 2.0f, attach, knee);
        addTriangle(verts, inds, foot,
            foot + glm::vec3(s * 0.02f, -0.02f, 0.03f),
            foot + glm::vec3(s * 0.01f, -0.01f, 0.01f),
            1.0f, 3.0f, 3.0f, attach, knee);
    }

    // ---------- 中脚 ----------
    {
        float s = -1.0f;
        glm::vec3 attach(s * 0.12f,  0.08f,  0.05f);
        glm::vec3 knee  (s * 0.28f,  0.02f,  0.00f);
        glm::vec3 foot  (s * 0.32f, -0.18f, -0.02f);
        addPrism(verts, inds, attach, knee, 0.018f, 1.0f, 1.0f, 1.0f, attach, knee);
        addPrism(verts, inds, knee,  foot,  0.014f, 1.0f, 1.0f, 2.0f, attach, knee);
        addTriangle(verts, inds, foot,
            foot + glm::vec3(s * 0.02f, -0.02f, 0.03f),
            foot + glm::vec3(s * 0.01f, -0.01f, 0.01f),
            1.0f, 1.0f, 3.0f, attach, knee);
    }
    {
        float s = +1.0f;
        glm::vec3 attach(s * 0.12f,  0.08f,  0.05f);
        glm::vec3 knee  (s * 0.28f,  0.02f,  0.00f);
        glm::vec3 foot  (s * 0.32f, -0.18f, -0.02f);
        addPrism(verts, inds, attach, knee, 0.018f, 1.0f, 4.0f, 1.0f, attach, knee);
        addPrism(verts, inds, knee,  foot,  0.014f, 1.0f, 4.0f, 2.0f, attach, knee);
        addTriangle(verts, inds, foot,
            foot + glm::vec3(s * 0.02f, -0.02f, 0.03f),
            foot + glm::vec3(s * 0.01f, -0.01f, 0.01f),
            1.0f, 4.0f, 3.0f, attach, knee);
    }

    // ---------- 後脚 (Z字型、太い、跳躍用) ----------
    {
        float s = -1.0f;
        glm::vec3 attach(s * 0.10f,  0.10f, -0.05f);
        glm::vec3 knee  (s * 0.20f,  0.30f, -0.30f);
        glm::vec3 foot  (s * 0.25f, -0.20f, -0.05f);
        addPrism(verts, inds, attach, knee, 0.035f, 1.0f, 2.0f, 1.0f, attach, knee);
        addPrism(verts, inds, knee,  foot,  0.018f, 1.0f, 2.0f, 2.0f, attach, knee);
        addTriangle(verts, inds, foot,
            foot + glm::vec3(s * 0.03f, -0.02f, 0.04f),
            foot + glm::vec3(s * 0.01f, -0.01f, 0.01f),
            1.0f, 2.0f, 3.0f, attach, knee);
    }
    {
        float s = +1.0f;
        glm::vec3 attach(s * 0.10f,  0.10f, -0.05f);
        glm::vec3 knee  (s * 0.20f,  0.30f, -0.30f);
        glm::vec3 foot  (s * 0.25f, -0.20f, -0.05f);
        addPrism(verts, inds, attach, knee, 0.035f, 1.0f, 5.0f, 1.0f, attach, knee);
        addPrism(verts, inds, knee,  foot,  0.018f, 1.0f, 5.0f, 2.0f, attach, knee);
        addTriangle(verts, inds, foot,
            foot + glm::vec3(s * 0.03f, -0.02f, 0.04f),
            foot + glm::vec3(s * 0.01f, -0.01f, 0.01f),
            1.0f, 5.0f, 3.0f, attach, knee);
    }

    // ---------- 一括スケール ----------
    for (size_t i = vStart; i < verts.size(); ++i) {
        verts[i].position  *= GH_SCALE;
        verts[i].pivotRoot *= GH_SCALE;
        verts[i].pivotKnee *= GH_SCALE;
    }
}

// ---------- main ----------
int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow("Grasshopper - 60mm Giant",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) return -1;

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) return -1;
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return -1;

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    glEnable(GL_DEPTH_TEST);

    std::string vShaderCode = loadShaderFromFile("vertex.glsl");
    std::string fShaderCode = loadShaderFromFile("fragment.glsl");
    if (vShaderCode.empty() || fShaderCode.empty()) return -1;
    const char* vertexShaderSource = vShaderCode.c_str();
    const char* fragmentShaderSource = fShaderCode.c_str();

    auto compile = [](GLenum type, const char* src) {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, NULL);
        glCompileShader(s);
        GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) { char log[1024]; glGetShaderInfoLog(s, 1024, NULL, log); std::cerr << log << std::endl; }
        return s;
    };
    GLuint vs = compile(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs); glLinkProgram(prog);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    generateGrasshopperMesh(vertices, indices);
    std::cout << "Vertices: " << vertices.size()
              << ", Indices: " << indices.size() << std::endl;

    GLuint VAO, VBO, EBO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));   glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, partID));   glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, legID));    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, legPartID));glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pivotRoot));glEnableVertexAttribArray(5);
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pivotKnee));glEnableVertexAttribArray(6);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, position)); glEnableVertexAttribArray(7); glVertexAttribDivisor(7, 1);
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, rotation)); glEnableVertexAttribArray(8); glVertexAttribDivisor(8, 1);
    glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, phase));    glEnableVertexAttribArray(9); glVertexAttribDivisor(9, 1);
    glVertexAttribPointer(10,1, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, speed));    glEnableVertexAttribArray(10); glVertexAttribDivisor(10, 1);

    std::vector<AntInstance> ghs(MAX_GH);
    srand(42);
    for (int i = 0; i < MAX_GH; i++) {
        ghs[i].position = glm::vec3(
            ((rand() % 1000) / 1000.0f - 0.5f) * 2.0f,
            0.0f,
            ((rand() % 1000) / 1000.0f - 0.5f) * 2.0f);
        ghs[i].rotation = (rand() % 1000) / 1000.0f * 6.28318f;
        ghs[i].phase    = (rand() % 1000) / 1000.0f * 6.28318f;
        ghs[i].speed    = 3.0f + (rand() % 1000) / 1000.0f * 3.0f;
    }
    glBufferData(GL_ARRAY_BUFFER, MAX_GH * sizeof(AntInstance), ghs.data(), GL_DYNAMIC_DRAW);

    Camera camera;
    camera.distance = 6.0f; // キリギリスは大きい

    GLint uTimeLoc        = glGetUniformLocation(prog, "uTime");
    GLint uMotionSpeedLoc = glGetUniformLocation(prog, "uMotionSpeed");
    GLint uViewProjLoc    = glGetUniformLocation(prog, "uViewProj");
    GLint uColorLoc       = glGetUniformLocation(prog, "uColor");

    bool running = true;
    Uint32 lastTime = SDL_GetTicks();
    float totalTime = 0.0f;
    int frameCount = 0;
    Uint32 fpsTimer = lastTime;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (event.key.keysym.sym == SDLK_UP) {
                    if (numGH < MAX_GH) { numGH++; std::cout << "GH Count: " << numGH << std::endl; }
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    if (numGH > 1) { numGH--; std::cout << "GH Count: " << numGH << std::endl; }
                }
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    motionSpeed = std::min(1.0f, motionSpeed + 0.1f);
                    std::cout << "Motion Speed: " << (motionSpeed * 100) << "%" << std::endl;
                }
                if (event.key.keysym.sym == SDLK_LEFT) {
                    motionSpeed = std::max(0.0f, motionSpeed - 0.1f);
                    std::cout << "Motion Speed: " << (motionSpeed * 100) << "%" << std::endl;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                camera.dragging = true; camera.lastX = event.button.x; camera.lastY = event.button.y;
            }
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) camera.dragging = false;
            if (event.type == SDL_MOUSEMOTION && camera.dragging) {
                camera.yaw -= (event.motion.x - camera.lastX) * 0.3f;
                camera.pitch += (event.motion.y - camera.lastY) * 0.3f;
                camera.pitch = glm::clamp(camera.pitch, -89.0f, 89.0f);
                camera.lastX = event.motion.x; camera.lastY = event.motion.y;
            }
            if (event.type == SDL_MOUSEWHEEL) {
                camera.distance -= event.wheel.y * 0.3f;
                camera.distance = glm::clamp(camera.distance, 1.0f, 30.0f);
            }
        }

        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTime) / 1000.0f;
        lastTime = now;
        totalTime += dt;

        for (int i = 0; i < numGH; i++) {
            AntInstance& a = ghs[i];
            float c = cos(a.rotation), s = sin(a.rotation);
            a.position.x += s * 0.5f * dt;
            a.position.z += c * 0.5f * dt;
            a.rotation += ((rand() % 100) / 100.0f - 0.5f) * 1.0f * dt;
            if (a.position.x >  1.5f) a.position.x = -1.5f;
            if (a.position.x < -1.5f) a.position.x =  1.5f;
            if (a.position.z >  1.5f) a.position.z = -1.5f;
            if (a.position.z < -1.5f) a.position.z =  1.5f;
        }
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numGH * sizeof(AntInstance), ghs.data());

        glClearColor(0.55f, 0.7f, 0.85f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1024.0f/768.0f, 0.1f, 200.0f);
        glm::mat4 view = camera.getView();
        glm::mat4 viewProj = proj * view;

        glUseProgram(prog);
        glUniform1f(uTimeLoc, totalTime);
        glUniform1f(uMotionSpeedLoc, motionSpeed);
        glUniformMatrix4fv(uViewProjLoc, 1, GL_FALSE, glm::value_ptr(viewProj));
        glUniform3f(uColorLoc, 0.18f, 0.38f, 0.12f); // 暗い緑 (絶望感)

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, numGH);

        SDL_GL_SwapWindow(window);

        frameCount++;
        if (now - fpsTimer >= 1000) {
            std::cout << "FPS: " << frameCount << " | GH: " << numGH
                      << " | Speed: " << (motionSpeed * 100) << "%" << std::endl;
            frameCount = 0;
            fpsTimer = now;
        }
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteProgram(prog);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
