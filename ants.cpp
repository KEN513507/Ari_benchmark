// ants.cpp - 100 ants with realistic ant mesh and tri-pod gait
#include "mesh_common.hpp"

const int MAX_ANTS = 200;
int numAnts = 100;
float motionSpeed = 1.0f; // 0.0 - 1.0

void generateAntMesh(std::vector<Vertex>& verts, std::vector<uint32_t>& inds) {
    glm::vec3 noPivot(0, 0, 0);

    // --- 胴体 (Body: partID = 0) ---
    // 頭部 (Head) - 楕円 (X: 0.13, Y: 0.10, Z: 0.15)
    addSphere(verts, inds, glm::vec3(0, 0.05f, 0.28f), glm::vec3(0.13f, 0.10f, 0.15f), 7, 5, 0.0f, -1.0f, 0.0f, noPivot, noPivot);
    // 胸部 (Thorax) - 楕円 (X: 0.12, Y: 0.12, Z: 0.16)
    addSphere(verts, inds, glm::vec3(0, 0.05f, 0.08f), glm::vec3(0.12f, 0.12f, 0.16f), 7, 5, 0.0f, -1.0f, 0.0f, noPivot, noPivot);
    // 腹柄 (Petiole/腰)
    addSphere(verts, inds, glm::vec3(0, 0.04f, -0.08f), glm::vec3(0.05f, 0.04f, 0.06f), 5, 3, 0.0f, -1.0f, 0.0f, noPivot, noPivot);
    // 腹部 (Abdomen) - 楕円 (X: 0.18, Y: 0.16, Z: 0.22)
    addSphere(verts, inds, glm::vec3(0, 0.08f, -0.28f), glm::vec3(0.18f, 0.16f, 0.22f), 7, 5, 0.0f, -1.0f, 0.0f, noPivot, noPivot);

    // --- 大顎 (Mandibles) - 薄く、内側に湾曲する挟む武器 ---
    // 左大顎
    {
        glm::vec3 base(-0.04f, 0.02f, 0.38f); // 付け根
        glm::vec3 knee(-0.08f, 0.00f, 0.44f); // 外側への曲がり
        glm::vec3 tip (-0.02f, 0.00f, 0.50f); // 内側へ向かう先端

        // 根元（太い部分）
        addPrism(verts, inds, base, knee, 0.008f, 0.0f, -1.0f, 0.0f, noPivot, noPivot);
        // 先端（薄い刃）
        addTriangle(verts, inds,
            knee,
            tip,
            knee + glm::vec3(0.0f, 0.02f, 0.0f), // 刃の高さ
            0.0f, -1.0f, 0.0f, noPivot, noPivot);
    }
    // 右大顎
    {
        glm::vec3 base( 0.04f, 0.02f, 0.38f);
        glm::vec3 knee( 0.08f, 0.00f, 0.44f);
        glm::vec3 tip ( 0.02f, 0.00f, 0.50f);

        addPrism(verts, inds, base, knee, 0.008f, 0.0f, -1.0f, 0.0f, noPivot, noPivot);
        addTriangle(verts, inds,
            knee,
            tip,
            knee + glm::vec3(0.0f, 0.02f, 0.0f),
            0.0f, -1.0f, 0.0f, noPivot, noPivot);
    }

    // --- 触角 (Antennae) - partID = 2、pivotRootを付け根に ---
    // 左触角 (legID = 0)
    {
        glm::vec3 base(-0.05f, 0.10f, 0.36f);
        glm::vec3 elbow(-0.14f, 0.20f, 0.46f);
        glm::vec3 tip (-0.08f, 0.16f, 0.60f);
        // pivotRoot = base に設定し、ここを中心に回転させる
        addPrism(verts, inds, base, elbow, 0.012f, 2.0f, 0.0f, 0.0f, base, noPivot);
        addPrism(verts, inds, elbow, tip, 0.009f, 2.0f, 0.0f, 0.0f, base, noPivot);
    }
    // 右触角 (legID = 1)
    {
        glm::vec3 base( 0.05f, 0.10f, 0.36f);
        glm::vec3 elbow( 0.14f, 0.20f, 0.46f);
        glm::vec3 tip ( 0.08f, 0.16f, 0.60f);
        addPrism(verts, inds, base, elbow, 0.012f, 2.0f, 1.0f, 0.0f, base, noPivot);
        addPrism(verts, inds, elbow, tip, 0.009f, 2.0f, 1.0f, 0.0f, base, noPivot);
    }

    // --- 脚 (Legs: partID = 1) ---
    struct LegDef {
        float side;     // -1 = Left, +1 = Right
        float zAttach;  // 付け根Z (胸部の表面)
        float zKnee;    // 膝Z
        float zFoot;    // 足先Z
        float legID;
    };

    // 前脚：前に伸びる / 中脚：後方へ開脚 / 後脚：さらに後方へ、関節は高く
    LegDef legs[6] = {
        {-1.0f,  0.18f,  0.24f,  0.32f, 0.0f}, // 左前 (legID=0)
        {+1.0f,  0.18f,  0.24f,  0.32f, 3.0f}, // 右前 (legID=3)
        {-1.0f, -0.02f, -0.12f, -0.24f, 1.0f}, // 左中 (legID=1) - 後方へ
        {+1.0f, -0.02f, -0.12f, -0.24f, 4.0f}, // 右中 (legID=4) - 後方へ
        {-1.0f, -0.10f, -0.22f, -0.36f, 2.0f}, // 左後 (legID=2) - さらに後方
        {+1.0f, -0.10f, -0.22f, -0.36f, 5.0f}, // 右後 (legID=5) - さらに後方
    };

    for (int i = 0; i < 6; i++) {
        float s = legs[i].side;
        float legID = legs[i].legID;

        glm::vec3 attach(s * 0.10f, 0.05f, legs[i].zAttach);

        // 脚ごとに膝の高さと張り出しを変える
        glm::vec3 knee;
        float femurRadius = 0.020f;
        if (legID == 2.0f || legID == 5.0f) {
            // 後脚: 大きな関節、胴体より高い位置 (Y=0.28)
            knee = glm::vec3(s * 0.42f, 0.28f, legs[i].zKnee);
            femurRadius = 0.024f; // 太く
        } else if (legID == 1.0f || legID == 4.0f) {
            // 中脚: やや高め
            knee = glm::vec3(s * 0.40f, 0.20f, legs[i].zKnee);
            femurRadius = 0.022f;
        } else {
            // 前脚
            knee = glm::vec3(s * 0.38f, 0.16f, legs[i].zKnee);
        }

        // 3. 足先 (外側・地面付近へ降ろす)
        glm::vec3 foot(s * 0.45f, -0.15f, legs[i].zFoot);

        // Femur (大腿部)
        addPrism(verts, inds, attach, knee, femurRadius, 1.0f, legID, 1.0f, attach, knee);
        // Tibia (脛部)
        addPrism(verts, inds, knee, foot, 0.015f, 1.0f, legID, 2.0f, attach, knee);
        // Tarsus (足先) - 三角形 (8頂点 -> 3頂点)
        glm::vec3 tarsusEnd = foot + glm::vec3(s * 0.03f, -0.02f, 0.02f);
        addTriangle(verts, inds,
            foot,
            tarsusEnd,
            foot + glm::vec3(s * 0.01f, -0.01f, 0.01f),
            1.0f, legID, 3.0f, attach, knee);
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow("Ant Prototype - Realistic Ant",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) return -1;

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) return -1;
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return -1;

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    glEnable(GL_DEPTH_TEST);

    // Compile shaders from files
    std::string vShaderCode = loadShaderFromFile("vertex.glsl");
    std::string fShaderCode = loadShaderFromFile("fragment.glsl");
    if (vShaderCode.empty() || fShaderCode.empty()) {
        std::cerr << "Shader files not found!" << std::endl;
        return -1;
    }
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

    // Generate mesh
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    generateAntMesh(vertices, indices);
    std::cout << "Vertices: " << vertices.size() << ", Indices: " << indices.size() << std::endl;

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

    // Instance attributes
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, position)); glEnableVertexAttribArray(7); glVertexAttribDivisor(7, 1);
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, rotation)); glEnableVertexAttribArray(8); glVertexAttribDivisor(8, 1);
    glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, phase));    glEnableVertexAttribArray(9); glVertexAttribDivisor(9, 1);
    glVertexAttribPointer(10,1, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, speed));    glEnableVertexAttribArray(10); glVertexAttribDivisor(10, 1);

    // Init instances
    std::vector<AntInstance> ants(MAX_ANTS);
    srand(42);
    for (int i = 0; i < MAX_ANTS; i++) {
        ants[i].position = glm::vec3(
            ((rand() % 1000) / 1000.0f - 0.5f) * 4.0f,
            0.0f,
            ((rand() % 1000) / 1000.0f - 0.5f) * 4.0f);
        ants[i].rotation = (rand() % 1000) / 1000.0f * 6.28318f;
        ants[i].phase    = (rand() % 1000) / 1000.0f * 6.28318f;
        ants[i].speed    = 4.0f + (rand() % 1000) / 1000.0f * 4.0f;
    }
    glBufferData(GL_ARRAY_BUFFER, MAX_ANTS * sizeof(AntInstance), ants.data(), GL_DYNAMIC_DRAW);

    Camera camera;
    GLint uTimeLoc = glGetUniformLocation(prog, "uTime");
    GLint uMotionSpeedLoc = glGetUniformLocation(prog, "uMotionSpeed");
    GLint uViewProjLoc = glGetUniformLocation(prog, "uViewProj");
    GLint uColorLoc = glGetUniformLocation(prog, "uColor");

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

                // 蟻の数の増減
                if (event.key.keysym.sym == SDLK_UP) {
                    if (numAnts < MAX_ANTS) {
                        numAnts++;
                        std::cout << "Ant Count: " << numAnts << std::endl;
                    }
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    if (numAnts > 1) {
                        numAnts--;
                        std::cout << "Ant Count: " << numAnts << std::endl;
                    }
                }

                // モーション速度の増減
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
                camera.distance = glm::clamp(camera.distance, 0.5f, 20.0f);
            }
        }

        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTime) / 1000.0f;
        lastTime = now;
        totalTime += dt;

        // CPU update: simple random walk
        for (int i = 0; i < numAnts; i++) {
            AntInstance& a = ants[i];
            float c = cos(a.rotation), s = sin(a.rotation);
            a.position.x += s * 0.3f * dt;
            a.position.z += c * 0.3f * dt;
            a.rotation += ((rand() % 100) / 100.0f - 0.5f) * 2.0f * dt;
            if (a.position.x >  2.0f) a.position.x = -2.0f;
            if (a.position.x < -2.0f) a.position.x =  2.0f;
            if (a.position.z >  2.0f) a.position.z = -2.0f;
            if (a.position.z < -2.0f) a.position.z =  2.0f;
        }
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, numAnts * sizeof(AntInstance), ants.data());

        // Render
        glClearColor(0.55f, 0.7f, 0.85f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1024.0f/768.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.getView();
        glm::mat4 viewProj = proj * view;

        glUseProgram(prog);
        glUniform1f(uTimeLoc, totalTime);
        glUniform1f(uMotionSpeedLoc, motionSpeed);
        glUniformMatrix4fv(uViewProjLoc, 1, GL_FALSE, glm::value_ptr(viewProj));
        glUniform3f(uColorLoc, 0.35f, 0.15f, 0.08f); // Ant brown (was hardcoded in fragment.glsl)

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, numAnts);

        SDL_GL_SwapWindow(window);

        // FPS display
        frameCount++;
        if (now - fpsTimer >= 1000) {
            std::cout << "FPS: " << frameCount << " | Ants: " << numAnts << " | Speed: " << (motionSpeed * 100) << "%" << std::endl;
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
