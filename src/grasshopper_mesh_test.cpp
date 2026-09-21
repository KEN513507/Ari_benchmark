// Grasshopper mesh regression tests.
// Uses the REAL buildGrasshopperMesh from grasshopper.cpp (included with its
// main() renamed away).
//  - antenna L/R mirror symmetry (partID == 2, legID 0 vs 1)
//  - wing triangles face up (partID == 3 normals must have +y)
//  - eyes protrude from the head and are L/R symmetric (partID == 5)
// Prints PASS lines; exit code 0 on success.
#include <cmath>
#include <cstdio>
#include <vector>

#define main grasshopper_disabled_main
#include "../grasshopper.cpp"
#undef main

namespace {

int failures = 0;

void check(bool cond, const char* name) {
    std::printf("%s %s\n", cond ? "PASS" : "FAIL", name);
    if (!cond) ++failures;
}

// Every vertex in A must have an unmatched x-mirror in B (and vice versa).
// Nearest-unused matching: first-fit greedily starves on duplicated vertices
// (sphere seams/poles), so always consume the closest available candidate.
bool setsMirrored(const std::vector<glm::vec3>& a, const std::vector<glm::vec3>& b,
                  float eps) {
    if (a.size() != b.size() || a.empty()) return false;
    std::vector<bool> used(b.size(), false);
    for (const auto& pa : a) {
        size_t best = b.size();
        float bestD = eps;
        for (size_t j = 0; j < b.size(); ++j) {
            if (used[j]) continue;
            float d = std::fabs(pa.x + b[j].x) + std::fabs(pa.y - b[j].y) +
                      std::fabs(pa.z - b[j].z);
            if (d < bestD) {
                bestD = d;
                best = j;
            }
        }
        if (best == b.size()) return false;
        used[best] = true;
    }
    return true;
}

void checkAntennae(const std::vector<Vertex>& verts, const char* name) {
    std::vector<glm::vec3> posR, posL, pivR, pivL;
    for (const auto& v : verts) {
        if (v.partID != 2.0f) continue;  // antennae only
        if (v.legID < 0.5f) {
            posR.push_back(v.position);
            pivR.push_back(v.pivotRoot);
        } else if (v.legID < 1.5f) {
            posL.push_back(v.position);
            pivL.push_back(v.pivotRoot);
        }
    }
    char buf[128];
    std::snprintf(buf, sizeof buf, "%s.antenna_present", name);
    check(!posR.empty() && posR.size() == posL.size(), buf);
    std::snprintf(buf, sizeof buf, "%s.antenna_positions_mirrored", name);
    check(setsMirrored(posR, posL, 1e-4f), buf);
    std::snprintf(buf, sizeof buf, "%s.antenna_pivots_mirrored", name);
    check(setsMirrored(pivR, pivL, 1e-4f), buf);
}

void checkWings(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds,
                const char* name) {
    int count = 0;
    bool allUp = true;
    for (size_t i = 0; i + 2 < inds.size(); i += 3) {
        const Vertex& a = verts[inds[i]];
        if (a.partID != 3.0f) continue;
        const Vertex& b = verts[inds[i + 1]];
        const Vertex& c = verts[inds[i + 2]];
        glm::vec3 n =
            glm::normalize(glm::cross(b.position - a.position, c.position - a.position));
        ++count;
        // 屋根型のため真上ではないが、上向きは必須 (傾斜 ~40° → y ≈ 0.77)
        if (n.y < 0.7f) allUp = false;
    }
    char buf[128];
    std::snprintf(buf, sizeof buf, "%s.wings_present", name);
    check(count == 4, buf);
    std::snprintf(buf, sizeof buf, "%s.wing_normals_up", name);
    check(allUp, buf);

    // 屋根型: 稜線 (|x| 小) が裾 (|x| 大) より明確に高い
    double sumSpine = 0.0, sumHem = 0.0;
    int nSpine = 0, nHem = 0;
    for (const auto& v : verts) {
        if (v.partID != 3.0f) continue;
        float ax = std::fabs(v.position.x);
        if (ax < 0.15f) {
            sumSpine += v.position.y;
            ++nSpine;
        } else if (ax > 0.5f) {
            sumHem += v.position.y;
            ++nHem;
        }
    }
    std::snprintf(buf, sizeof buf, "%s.wings_form_roof", name);
    check(nSpine > 0 && nHem > 0 &&
          (sumSpine / nSpine) > (sumHem / nHem) + 0.3, buf);
}

void checkHindFemur(const std::vector<Vertex>& verts, const char* name) {
    // 後脚 (legID 2/5) Femur (legPartID 1): 根元側が膝側より明確に太い
    double sumRoot = 0.0, sumKnee = 0.0;
    int nRoot = 0, nKnee = 0;
    for (const auto& v : verts) {
        if (v.partID != 1.0f || v.legPartID != 1.0f) continue;
        if (v.legID != 2.0f && v.legID != 5.0f) continue;
        glm::vec3 axis = v.pivotKnee - v.pivotRoot;
        float len2 = glm::dot(axis, axis);
        if (len2 < 1e-6f) continue;
        float t = glm::dot(v.position - v.pivotRoot, axis) / len2;
        glm::vec3 onAxis = v.pivotRoot + axis * t;
        float r = glm::length(v.position - onAxis);
        if (t < 0.35f) {
            sumRoot += r;
            ++nRoot;
        } else if (t > 0.65f) {
            sumKnee += r;
            ++nKnee;
        }
    }
    char buf[128];
    std::snprintf(buf, sizeof buf, "%s.hindfemur_present", name);
    check(nRoot > 0 && nKnee > 0, buf);
    std::snprintf(buf, sizeof buf, "%s.hindfemur_tapered", name);
    check(nRoot > 0 && nKnee > 0 && (sumRoot / nRoot) > (sumKnee / nKnee) * 1.5f,
          buf);
}

void checkEyes(const std::vector<Vertex>& verts, const char* name) {
    std::vector<glm::vec3> eyeR, eyeL;
    for (const auto& v : verts) {
        if (v.partID != 5.0f) continue;  // eyes only
        if (v.position.x > 0.0f) {
            eyeR.push_back(v.position);
        } else {
            eyeL.push_back(v.position);
        }
    }
    char buf[128];
    std::snprintf(buf, sizeof buf, "%s.eyes_present_both_sides", name);
    check(!eyeR.empty() && eyeR.size() == eyeL.size(), buf);
    std::snprintf(buf, sizeof buf, "%s.eyes_mirrored", name);
    check(setsMirrored(eyeR, eyeL, 1e-4f), buf);
    // Head body half-width is ~0.525; eyes must stick out beyond it.
    float eyeMaxX = 0.0f;
    for (const auto& p : eyeR) eyeMaxX = std::fmax(eyeMaxX, p.x);
    std::snprintf(buf, sizeof buf, "%s.eyes_protrude_from_head", name);
    check(eyeMaxX > 0.60f, buf);
}

void checkPose(const GrasshopperPose& pose, const char* name) {
    std::vector<Vertex> verts;
    std::vector<uint32_t> inds;
    buildGrasshopperMesh(pose, verts, inds);
    checkAntennae(verts, name);
    checkWings(verts, inds, name);
    checkEyes(verts, name);
    checkHindFemur(verts, name);
}

}  // namespace

int main() {
    GrasshopperPose rest;
    for (int i = 0; i < 6; i++) rest.legFoot[i] = FOOT_ANCHORS[i];
    checkPose(rest, "rest");

    const float phases[] = { 0.0f, 0.3f, 0.5f, 0.7f, 0.9f };
    for (float ph : phases) {
        char buf[64];
        std::snprintf(buf, sizeof buf, "rush@%.1f", ph);
        checkPose(computeRushPose(ph), buf);
    }

    if (failures == 0) std::printf("ALL_PASS\n");
    return failures ? 1 : 0;
}
