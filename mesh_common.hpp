// mesh_common.hpp - 蟻とキリギリスで共有するメッシュ・カメラ・IO
#pragma once

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstddef>

// ---------- 頂点属性 ----------
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    float partID;
    float legID;
    float legPartID;
    glm::vec3 pivotRoot;
    glm::vec3 pivotKnee;
};

// ---------- インスタンス属性 ----------
struct AntInstance {
    glm::vec3 position;
    float rotation;
    float phase;
    float speed;
};

// ---------- シェーダ読み込み ----------
inline std::string loadShaderFromFile(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ---------- 楕円体 (球体) ----------
inline void addSphere(std::vector<Vertex>& verts, std::vector<uint32_t>& inds,
                      glm::vec3 center, glm::vec3 radii, int slices, int stacks,
                      float partID, float legID, float legPartID,
                      glm::vec3 pivotRoot, glm::vec3 pivotKnee) {
    uint32_t base = (uint32_t)verts.size();
    for (int i = 0; i <= stacks; ++i) {
        float phi = glm::pi<float>() * float(i) / float(stacks);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * glm::pi<float>() * float(j) / float(slices);
            glm::vec3 n = glm::vec3(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));
            Vertex v;
            v.position = center + n * radii;
            v.normal = n;
            v.partID = partID;
            v.legID = legID;
            v.legPartID = legPartID;
            v.pivotRoot = pivotRoot;
            v.pivotKnee = pivotKnee;
            verts.push_back(v);
        }
    }
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            uint32_t a = base + i * (slices + 1) + j;
            uint32_t b = base + i * (slices + 1) + j + 1;
            uint32_t c = base + (i + 1) * (slices + 1) + j;
            uint32_t d = base + (i + 1) * (slices + 1) + j + 1;
            inds.push_back(a); inds.push_back(c); inds.push_back(b);
            inds.push_back(b); inds.push_back(c); inds.push_back(d);
        }
    }
}

// ---------- 四角柱 ----------
inline void addPrism(std::vector<Vertex>& verts, std::vector<uint32_t>& inds,
                     glm::vec3 start, glm::vec3 end, float radius,
                     float partID, float legID, float legPartID,
                     glm::vec3 pivotRoot, glm::vec3 pivotKnee) {
    glm::vec3 dir = end - start;
    float len = glm::length(dir);
    if (len < 0.001f) return;
    glm::vec3 dirN = dir / len;

    glm::vec3 up(0, 1, 0);
    if (std::abs(glm::dot(dirN, up)) > 0.99f) up = glm::vec3(1, 0, 0);
    glm::vec3 right = glm::normalize(glm::cross(dirN, up));
    glm::vec3 fwd = glm::normalize(glm::cross(right, dirN));

    glm::vec3 offsets[4] = {
        right * radius, fwd * radius, -right * radius, -fwd * radius
    };

    uint32_t base = (uint32_t)verts.size();
    for (int i = 0; i < 4; ++i) {
        glm::vec3 n = glm::normalize(offsets[i]);
        Vertex v1, v2;
        v1.position = start + offsets[i]; v1.normal = n;
        v2.position = end + offsets[i];   v2.normal = n;
        v1.partID = v2.partID = partID;
        v1.legID = v2.legID = legID;
        v1.legPartID = v2.legPartID = legPartID;
        v1.pivotRoot = v2.pivotRoot = pivotRoot;
        v1.pivotKnee = v2.pivotKnee = pivotKnee;
        verts.push_back(v1);
        verts.push_back(v2);
    }
    for (int i = 0; i < 4; ++i) {
        int next = (i + 1) % 4;
        uint32_t a = base + i * 2;
        uint32_t b = base + i * 2 + 1;
        uint32_t c = base + next * 2;
        uint32_t d = base + next * 2 + 1;
        inds.push_back(a); inds.push_back(c); inds.push_back(b);
        inds.push_back(b); inds.push_back(c); inds.push_back(d);
    }
}

// ---------- テーパー付き四角柱 (後脚 Femur の筋肉テーパー用) ----------
// 断面は正方形のまま root 側 rStart から先端 rEnd へ線形に絞る。
// addPrism と同じ面構成 (side 毎ストリップ) で、法線はテーパー傾斜を補正する。
inline void addTaperedPrism(std::vector<Vertex>& verts, std::vector<uint32_t>& inds,
                     glm::vec3 start, glm::vec3 end, float rStart, float rEnd, int segments,
                     float partID, float legID, float legPartID,
                     glm::vec3 pivotRoot, glm::vec3 pivotKnee) {
    if (segments < 1) segments = 1;
    glm::vec3 dir = end - start;
    float len = glm::length(dir);
    if (len < 0.001f) return;
    glm::vec3 dirN = dir / len;

    glm::vec3 up(0, 1, 0);
    if (std::abs(glm::dot(dirN, up)) > 0.99f) up = glm::vec3(1, 0, 0);
    glm::vec3 right = glm::normalize(glm::cross(dirN, up));
    glm::vec3 fwd = glm::normalize(glm::cross(right, dirN));
    glm::vec3 side[4] = {right, fwd, -right, -fwd};

    float slope = (rStart - rEnd) / len;
    // 全 side のストリップを先に生成 (addPrism と同じ面構成にするため)
    uint32_t strip[4];
    for (int s = 0; s < 4; s++) {
        glm::vec3 n = glm::normalize(side[s] + dirN * slope);
        strip[s] = (uint32_t)verts.size();
        for (int i = 0; i <= segments; i++) {
            float t = (float)i / (float)segments;
            glm::vec3 c = start + dir * t;
            float r = rStart + (rEnd - rStart) * t;
            Vertex v;
            v.position = c + side[s] * r;
            v.normal = n;
            v.partID = partID;
            v.legID = legID;
            v.legPartID = legPartID;
            v.pivotRoot = pivotRoot;
            v.pivotKnee = pivotKnee;
            verts.push_back(v);
        }
    }
    for (int s = 0; s < 4; s++) {
        int sn = (s + 1) % 4;
        for (int i = 0; i < segments; i++) {
            uint32_t a = strip[s] + i;
            uint32_t b = strip[s] + i + 1;
            uint32_t c = strip[sn] + i;
            uint32_t d = strip[sn] + i + 1;
            inds.push_back(a); inds.push_back(c); inds.push_back(b);
            inds.push_back(b); inds.push_back(c); inds.push_back(d);
        }
    }
}

// ---------- 三角形 ----------
inline void addTriangle(std::vector<Vertex>& verts, std::vector<uint32_t>& inds,
                        glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
                        float partID, float legID, float legPartID,
                        glm::vec3 pivotRoot, glm::vec3 pivotKnee) {
    uint32_t base = (uint32_t)verts.size();
    glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

    Vertex v1, v2, v3;
    v1.position = p1; v1.normal = normal;
    v2.position = p2; v2.normal = normal;
    v3.position = p3; v3.normal = normal;

    v1.partID = v2.partID = v3.partID = partID;
    v1.legID = v2.legID = v3.legID = legID;
    v1.legPartID = v2.legPartID = v3.legPartID = legPartID;
    v1.pivotRoot = v2.pivotRoot = v3.pivotRoot = pivotRoot;
    v1.pivotKnee = v2.pivotKnee = v3.pivotKnee = pivotKnee;

    verts.push_back(v1); verts.push_back(v2); verts.push_back(v3);
    inds.push_back(base); inds.push_back(base+1); inds.push_back(base+2);
}

// ---------- 変形可能な箱 (bodyM などの変換行列を受け取る) ----------
inline void addBoxM(std::vector<Vertex>& verts, std::vector<uint32_t>& inds,
                    const glm::mat4& M,
                    glm::vec3 center, glm::vec3 size,
                    float partID, float legID, float legPartID,
                    glm::vec3 pivotRoot, glm::vec3 pivotKnee) {
    glm::vec3 h = size * 0.5f;
    glm::vec3 localC[8] = {
        center + glm::vec3(-h.x, -h.y, -h.z),
        center + glm::vec3( h.x, -h.y, -h.z),
        center + glm::vec3( h.x,  h.y, -h.z),
        center + glm::vec3(-h.x,  h.y, -h.z),
        center + glm::vec3(-h.x, -h.y,  h.z),
        center + glm::vec3( h.x, -h.y,  h.z),
        center + glm::vec3( h.x,  h.y,  h.z),
        center + glm::vec3(-h.x,  h.y,  h.z),
    };
    glm::vec3 localN[6] = {{0,0,-1},{0,0,1},{-1,0,0},{1,0,0},{0,-1,0},{0,1,0}};
    int f[6][4] = {{0,3,2,1},{4,5,6,7},{0,4,7,3},{1,2,6,5},{0,1,5,4},{3,7,6,2}};

    glm::mat3 rotM = glm::mat3(M);

    uint32_t base = (uint32_t)verts.size();
    for (int fi = 0; fi < 6; fi++) {
        glm::vec3 worldN = glm::normalize(rotM * localN[fi]);
        for (int v = 0; v < 4; v++) {
            Vertex vert;
            vert.position  = glm::vec3(M * glm::vec4(localC[f[fi][v]], 1.0f));
            vert.normal    = worldN;
            vert.partID    = partID;
            vert.legID     = legID;
            vert.legPartID = legPartID;
            vert.pivotRoot = glm::vec3(M * glm::vec4(pivotRoot, 1.0f));
            vert.pivotKnee = glm::vec3(M * glm::vec4(pivotKnee, 1.0f));
            verts.push_back(vert);
        }
        uint32_t i0 = base + fi*4 + 0;
        uint32_t i1 = base + fi*4 + 1;
        uint32_t i2 = base + fi*4 + 2;
        uint32_t i3 = base + fi*4 + 3;
        inds.push_back(i0); inds.push_back(i1); inds.push_back(i2);
        inds.push_back(i0); inds.push_back(i2); inds.push_back(i3);
    }
}

// ---------- カメラ ----------
struct Camera {
    float yaw = 45.0f, pitch = 30.0f, distance = 3.0f;
    glm::vec3 target = glm::vec3(0, 0, 0);
    bool dragging = false;
    int lastX = 0, lastY = 0;

    glm::mat4 getView() const {
        float ry = glm::radians(yaw), rp = glm::radians(pitch);
        glm::vec3 pos = target + glm::vec3(
            distance * cos(rp) * sin(ry),
            distance * sin(rp),
            distance * cos(rp) * cos(ry));
        return glm::lookAt(pos, target, glm::vec3(0, 1, 0));
    }
};
