// grasshopper.cpp - キリギリス捕食シミュレータ (HTML版のC++移植)
#include "mesh_common.hpp"
#include <cstdio>

// ============================================================
// Pose (姿勢) - 毎フレームこれを作り、メッシュを再構築する
// ============================================================
struct GrasshopperPose {
    float bodyY       = 1.10f;
    float bodyZ       = 0.00f;
    float bodyPitch   = 0.0f;   // rad
    float bodyYaw     = 0.0f;
    float headPitch   = 0.0f;
    float mandibleAngle = 0.15f;
    glm::vec3 legFoot[6];       // 各脚の接地目標位置 (world space)
    bool legVisible[6] = {true,true,true,true,true,true};
};

// ============================================================
// 2-bone IK: root と foot から knee 位置を決定
// ============================================================
glm::vec3 solveKnee(const glm::vec3& root, const glm::vec3& foot,
                    float l1, float l2, const glm::vec3& bendDir) {
    glm::vec3 rt = foot - root;
    float d = glm::length(rt);
    if (d < 0.05f) d = 0.05f;
    float maxD = (l1 + l2) * 0.999f;
    if (d > maxD) d = maxD;

    glm::vec3 axis = rt / glm::length(rt);
    float cosAlpha = (l1*l1 + d*d - l2*l2) / (2.0f * l1 * d);
    cosAlpha = glm::clamp(cosAlpha, -1.0f, 1.0f);
    float alpha = std::acos(cosAlpha);

    glm::vec3 planeNorm = glm::cross(axis, bendDir);
    if (glm::length(planeNorm) < 0.001f) {
        planeNorm = glm::cross(glm::vec3(0,1,0), axis);
    }
    planeNorm = glm::normalize(planeNorm);

    glm::mat4 rotM = glm::rotate(glm::mat4(1.0f), alpha, planeNorm);
    glm::vec3 kneeDir = glm::vec3(rotM * glm::vec4(axis, 0.0f));
    return root + kneeDir * l1;
}

// ============================================================
// 脚の定義 (HTML から移植)
// ============================================================
struct LegDef {
    int   legID;
    float side;              // -1 or +1
    glm::vec3 rootLocal;     // body-local attach point
    float lF, lT;            // Femur/Tibia length
    float rF, rT;            // Femur/Tibia radius
    glm::vec3 bendDir;       // knee bending direction (body-local)
    bool  isHind;
};

const LegDef LEGS[6] = {
    // Front
    {0, -1.0f, {-0.55f, 0.55f,  3.00f}, 0.9f, 1.4f, 0.11f, 0.08f, {-1.0f, 0.5f,  0.5f}, false},
    {3, +1.0f, { 0.55f, 0.55f,  3.00f}, 0.9f, 1.4f, 0.11f, 0.08f, { 1.0f, 0.5f,  0.5f}, false},
    // Mid
    {1, -1.0f, {-0.65f, 0.50f,  1.70f}, 1.0f, 1.5f, 0.11f, 0.08f, {-1.0f, 0.5f,  0.1f}, false},
    {4, +1.0f, { 0.65f, 0.50f,  1.70f}, 1.0f, 1.5f, 0.11f, 0.08f, { 1.0f, 0.5f,  0.1f}, false},
    // Hind (Femur は胴体上面 0.85 より遥か上まで伸びる)
    {2, -1.0f, {-0.65f, 0.55f,  0.80f}, 2.60f, 3.00f, 0.30f, 0.13f, {-0.6f, 1.4f, -0.9f}, true},
    {5, +1.0f, { 0.65f, 0.55f,  0.80f}, 2.60f, 3.00f, 0.30f, 0.13f, { 0.6f, 1.4f, -0.9f}, true},
};

const glm::vec3 FOOT_ANCHORS[6] = {
    // Front: 前方・外側
    {-1.50f, 0.0f,  4.10f},
    { 1.50f, 0.0f,  4.10f},
    // Mid: 真横
    {-2.00f, 0.0f,  1.60f},
    { 2.00f, 0.0f,  1.60f},
    // Hind: 後方・外側
    {-1.30f, 0.0f, -2.30f},
    { 1.30f, 0.0f, -2.30f},
};

// ============================================================
// Pose -> Mesh 変換 (毎フレーム呼ばれる)
// ============================================================
void buildGrasshopperMesh(const GrasshopperPose& pose,
                          std::vector<Vertex>& verts,
                          std::vector<uint32_t>& inds) {
    verts.clear();
    inds.clear();

    glm::vec3 noPivot(0, 0, 0);

    glm::mat4 bodyM = glm::translate(glm::mat4(1.0f), glm::vec3(0, pose.bodyY, pose.bodyZ));
    bodyM = glm::rotate(bodyM, pose.bodyYaw, glm::vec3(0,1,0));
    bodyM = glm::rotate(bodyM, pose.bodyPitch, glm::vec3(1,0,0));

    auto W = [&](const glm::vec3& p) { return glm::vec3(bodyM * glm::vec4(p, 1.0f)); };
    auto D = [&](const glm::vec3& d) { return glm::vec3(bodyM * glm::vec4(d, 0.0f)); };

    // ローカル座標系で箱を生成するヘルパ
    auto Box = [&](glm::vec3 c, glm::vec3 s, float pid, float lid, float lpid) {
        addBoxM(verts, inds, bodyM, c, s, pid, lid, lpid, noPivot, noPivot);
    };

    // Z軸回りに傾けた箱 (前胸背板の側裙など)。angZ>0 で上端が -x 側へ傾く。
    auto TiltBox = [&](glm::vec3 c, glm::vec3 s, float angZ,
                       float pid, float lid, float lpid) {
        glm::mat4 M = bodyM
            * glm::translate(glm::mat4(1.0f), c)
            * glm::rotate(glm::mat4(1.0f), angZ, glm::vec3(0, 0, 1))
            * glm::translate(glm::mat4(1.0f), -c);
        addBoxM(verts, inds, M, c, s, pid, lid, lpid, noPivot, noPivot);
    };

    // ============================================================
    // 頭部 — 下口式 (口器が真下を向くウェッジ型)
    // ============================================================
    // 後頭部（胸部に密着）
    Box({0, 0.40f, 3.55f}, {1.00f, 0.85f, 0.30f}, 0.0f, -1.0f, 0.0f);
    // 頭蓋 (cranium)
    Box({0, 0.55f, 4.05f}, {1.00f, 0.80f, 0.90f}, 0.0f, -1.0f, 0.0f);
    // 顔面: 下方ほど後退する傾斜 (前面 4.50 → 4.345)
    Box({0, 0.02f, 4.12f}, {0.68f, 0.55f, 0.45f}, 0.0f, -1.0f, 0.0f);
    // 下唇 (labium、口器の後壁)
    Box({0, -0.32f, 4.05f}, {0.30f, 0.35f, 0.25f}, 0.0f, -1.0f, 0.0f);
    // 複眼: 球状に側面上部へ突出 (partID=5 で暗色に塗る)
    addSphere(verts, inds, W({ 0.52f, 0.72f, 4.02f}),
              {0.30f, 0.30f, 0.32f}, 8, 6, 5.0f, -1.0f, 0.0f, noPivot, noPivot);
    addSphere(verts, inds, W({-0.52f, 0.72f, 4.02f}),
              {0.30f, 0.30f, 0.32f}, 8, 6, 5.0f, -1.0f, 0.0f, noPivot, noPivot);

    // 大顎 (mandibleAngle で開閉、真下〜やや後ろを向く)
    {
        glm::vec3 pivotL = W({ 0.20f, -0.20f, 4.15f});
        glm::vec3 pivotR = W({-0.20f, -0.20f, 4.15f});
        float a = pose.mandibleAngle;

        glm::vec3 tipL = W({ 0.20f + std::sin(a)*0.18f, -0.20f - std::cos(a)*0.70f, 4.12f - std::cos(a)*0.10f});
        glm::vec3 tipR = W({-0.20f - std::sin(a)*0.18f, -0.20f - std::cos(a)*0.70f, 4.12f - std::cos(a)*0.10f});

        addPrism(verts, inds, pivotL, tipL, 0.10f, 4.0f, 0.0f, 0.0f, pivotL, noPivot);
        addPrism(verts, inds, pivotR, tipR, 0.10f, 4.0f, 1.0f, 0.0f, pivotR, noPivot);
    }

    // ============================================================
    // 触角 — 四角柱3節で、なめらかな弧
    // ============================================================
    {
        glm::vec3 b  = W({ 0.28f, 0.92f, 4.42f});
        glm::vec3 m1 = W({ 0.55f, 1.70f, 5.55f});
        glm::vec3 m2 = W({ 0.70f, 2.90f, 6.20f});
        glm::vec3 e  = W({ 0.50f, 4.20f, 6.50f});
        addPrism(verts, inds, b,  m1, 0.055f, 2.0f, 0.0f, 0.0f, b, noPivot);
        addPrism(verts, inds, m1, m2, 0.045f, 2.0f, 0.0f, 0.0f, b, noPivot);
        addPrism(verts, inds, m2, e,  0.030f, 2.0f, 0.0f, 0.0f, b, noPivot);
    }
    {
        glm::vec3 b  = W({-0.28f, 0.92f, 4.42f});
        glm::vec3 m1 = W({-0.55f, 1.70f, 5.55f});
        glm::vec3 m2 = W({-0.70f, 2.90f, 6.20f});
        glm::vec3 e  = W({-0.50f, 4.20f, 6.50f});
        addPrism(verts, inds, b,  m1, 0.055f, 2.0f, 1.0f, 0.0f, b, noPivot);
        addPrism(verts, inds, m1, m2, 0.045f, 2.0f, 1.0f, 0.0f, b, noPivot);
        addPrism(verts, inds, m2, e,  0.030f, 2.0f, 1.0f, 0.0f, b, noPivot);
    }

    // ============================================================
    // 胸部 (四角) — 上面に鞍、本体は横長の箱
    // ============================================================
    // Pronotum (前胸背板): 胸部を覆う鞍型シェル
    // 天板 (首〜腹部分節前端まで覆う)
    Box({0, 1.00f, 2.30f}, {1.10f, 0.30f, 1.90f}, 0.0f, -1.0f, 0.0f);
    // 後葉 (腹部分節の前端に覆いかぶさる)
    Box({0, 0.88f, 1.20f}, {0.95f, 0.26f, 0.55f}, 0.0f, -1.0f, 0.0f);
    // 側裙 (サイドスカート): 上端を天板の下に潜り込ませ、下端を外へ張り出す
    TiltBox({ 0.60f, 0.60f, 2.30f}, {0.16f, 0.82f, 1.75f},  0.30f, 0.0f, -1.0f, 0.0f);
    TiltBox({-0.60f, 0.60f, 2.30f}, {0.16f, 0.82f, 1.75f}, -0.30f, 0.0f, -1.0f, 0.0f);
    // 胸部本体
    Box({0, 0.45f, 2.10f}, {1.30f, 0.95f, 1.55f}, 0.0f, -1.0f, 0.0f);
    // 胸部 → 腹部接続
    Box({0, 0.45f, 1.05f}, {1.10f, 0.85f, 0.70f}, 0.0f, -1.0f, 0.0f);

    // ============================================================
    // 腹部 (四角) — 8節、後方テーパー、横縞
    // ============================================================
    for (int i = 0; i < 8; i++) {
        float t = i / 7.0f;
        float z = 0.45f - i * 0.62f;
        float w = 1.00f - t * 0.55f;      // 幅: 1.00 → 0.45
        float h = 0.85f - t * 0.40f;      // 高さ: 0.85 → 0.45
        float y = 0.40f + t * 0.20f;      // 緩やかに上昇
        Box({0, y, z}, {w, h, 0.55f}, 0.0f, -1.0f, 0.0f);
    }
    // 腹部末端（尖る）
    Box({0, 0.65f, -4.75f}, {0.35f, 0.35f, 0.30f}, 0.0f, -1.0f, 0.0f);

    // ============================================================
    // 翅 (partID=3) — 背の稜線から側面へ垂れる屋根型シェル。
    // 腹部分節を左右から包み、断面を三角形 (合掌造り) にする。
    // 巻き順注意: 法線が上向きになるよう左右で合わせること。
    // ============================================================
    {
        // 左翅: A=前稜 B=前裾 C=後裾 D=後稜
        addTriangle(verts, inds,
            W({-0.06f, 1.00f,  1.55f}),
            W({-0.70f, 0.38f, -4.60f}),
            W({-0.80f, 0.42f,  1.30f}),
            3.0f, 0.0f, 0.0f, noPivot, noPivot);
        addTriangle(verts, inds,
            W({-0.06f, 1.00f,  1.55f}),
            W({-0.05f, 0.86f, -4.75f}),
            W({-0.70f, 0.38f, -4.60f}),
            3.0f, 0.0f, 0.0f, noPivot, noPivot);
    }
    {
        // 右翅
        addTriangle(verts, inds,
            W({ 0.06f, 1.00f,  1.55f}),
            W({ 0.80f, 0.42f,  1.30f}),
            W({ 0.70f, 0.38f, -4.60f}),
            3.0f, 1.0f, 0.0f, noPivot, noPivot);
        addTriangle(verts, inds,
            W({ 0.06f, 1.00f,  1.55f}),
            W({ 0.70f, 0.38f, -4.60f}),
            W({ 0.05f, 0.86f, -4.75f}),
            3.0f, 1.0f, 0.0f, noPivot, noPivot);
    }

    // ============================================================
    // 脚 (IK) — テーパー腿 + 関節ノブ + 足裏パッド
    // ============================================================
    glm::mat4 I4(1.0f);
    for (int i = 0; i < 6; i++) {
        if (!pose.legVisible[i]) continue;
        const LegDef& L = LEGS[i];
        glm::vec3 rootW = W(L.rootLocal);
        glm::vec3 footW = pose.legFoot[i];
        glm::vec3 bendW = glm::normalize(D(L.bendDir));

        glm::vec3 kneeW = solveKnee(rootW, footW, L.lF, L.lT, bendW);

        if (L.isHind) {
            // 基節ソケット (胸部側面に埋まる付け根ブロック)
            addBoxM(verts, inds, I4, rootW, glm::vec3(0.36f, 0.32f, 0.42f),
                    1.0f, L.legID, 0.0f, rootW, kneeW);
            // Femur: 根元が太く膝へ絞られる筋肉テーパー (2区間で非線形に)
            glm::vec3 midF = rootW + (kneeW - rootW) * 0.45f;
            addTaperedPrism(verts, inds, rootW, midF, 0.44f, 0.30f, 2,
                            1.0f, L.legID, 1.0f, rootW, kneeW);
            addTaperedPrism(verts, inds, midF, kneeW, 0.30f, 0.15f, 2,
                            1.0f, L.legID, 1.0f, rootW, kneeW);
            // 膝関節ノブ
            addBoxM(verts, inds, I4, kneeW, glm::vec3(0.26f, 0.26f, 0.26f),
                    1.0f, L.legID, 2.0f, rootW, kneeW);
            // Tibia: 細身テーパー
            addTaperedPrism(verts, inds, kneeW, footW, 0.12f, 0.06f, 2,
                            1.0f, L.legID, 2.0f, rootW, kneeW);
        } else {
            addBoxM(verts, inds, I4, rootW, glm::vec3(0.22f, 0.20f, 0.28f),
                    1.0f, L.legID, 0.0f, rootW, kneeW);
            addTaperedPrism(verts, inds, rootW, kneeW, 0.16f, 0.10f, 1,
                            1.0f, L.legID, 1.0f, rootW, kneeW);
            addBoxM(verts, inds, I4, kneeW, glm::vec3(0.16f, 0.16f, 0.16f),
                    1.0f, L.legID, 2.0f, rootW, kneeW);
            addTaperedPrism(verts, inds, kneeW, footW, 0.09f, 0.05f, 1,
                            1.0f, L.legID, 2.0f, rootW, kneeW);
        }

        // 跗節: 前方へ伸びる趾 + 地面に面で接地する足裏パッド
        glm::vec3 fwdW = D(glm::vec3(0, 0, 1));
        fwdW.y = 0.0f;
        if (glm::length(fwdW) < 0.01f) fwdW = glm::vec3(0, 0, 1);
        fwdW = glm::normalize(fwdW);
        glm::vec3 latW = glm::normalize(glm::cross(glm::vec3(0, 1, 0), fwdW));
        glm::vec3 toeW = footW + fwdW * 0.30f + latW * (L.side * 0.06f);
        toeW.y = 0.06f;
        addTaperedPrism(verts, inds, footW, toeW, 0.05f, 0.03f, 1,
                        1.0f, L.legID, 3.0f, rootW, kneeW);
        glm::vec3 padC = toeW + fwdW * 0.10f;
        padC.y = 0.035f;
        addBoxM(verts, inds, I4, padC, glm::vec3(0.20f, 0.07f, 0.38f),
                1.0f, L.legID, 3.0f, rootW, kneeW);
    }
}
// ============================================================
// アニメーション: フェーズ -> Pose
// ============================================================
enum class AttackMode { RUSH, SWEEP };

GrasshopperPose computeRushPose(float phase) {
    GrasshopperPose p;
    for (int i = 0; i < 6; i++) p.legFoot[i] = FOOT_ANCHORS[i];

    if (phase < 0.25f) {
        p.bodyY = 1.20f;
        p.bodyZ = 0.0f;
        p.mandibleAngle = 0.15f;
    } else if (phase < 0.45f) {
        float u = (phase - 0.25f) / 0.20f;
        p.bodyY = 1.20f + u * 0.10f;
        p.bodyZ = -u * 0.25f;
        p.bodyPitch = -u * 0.08f;
        p.headPitch = -u * 0.05f;
        p.mandibleAngle = 0.15f + u * 0.45f;
    } else if (phase < 0.60f) {
        p.bodyY = 1.30f;
        p.bodyZ = -0.25f;
        p.bodyPitch = -0.08f;
        p.headPitch = -0.05f;
        p.mandibleAngle = 0.60f;
    } else if (phase < 0.82f) {
        float u = (phase - 0.60f) / 0.22f;
        float plunge = std::sin(u * glm::pi<float>() * 0.5f);
        p.bodyY       = glm::mix(1.30f, 1.05f, plunge);   // 頭が埋まらない最低高
        p.bodyZ       = glm::mix(-0.25f, 1.65f, plunge);
        p.bodyPitch   = glm::mix(-0.08f, 0.22f, plunge);  // 前傾 12.6° まで
        p.headPitch   = glm::mix(-0.05f, 0.18f, plunge);  // 頭の前傾 10° まで
        p.mandibleAngle = (u > 0.65f) ? 0.02f : 0.65f;
    } else {
        float u = (phase - 0.82f) / 0.18f;
        p.bodyY = glm::mix(1.05f, 1.20f, u);
        p.bodyZ = glm::mix(1.65f, 0.00f, u);
        p.bodyPitch = glm::mix(0.22f, 0.00f, u);
        p.headPitch = glm::mix(0.18f, 0.00f, u);
        p.mandibleAngle = 0.15f;
    }
    return p;
}

GrasshopperPose computeSweepPose(float phase) {
    GrasshopperPose p;
    for (int i = 0; i < 6; i++) p.legFoot[i] = FOOT_ANCHORS[i];

    if (phase < 0.25f) {
        float u = phase / 0.25f;
        p.bodyY = 1.15f;
        p.bodyYaw = -u * 0.28f;
        p.bodyZ = -u * 0.20f;
    } else if (phase < 0.45f) {
        float u = (phase - 0.25f) / 0.20f;
        p.bodyY = 1.25f;
        p.bodyYaw = -0.28f - u * 0.10f;
        p.bodyPitch = -u * 0.15f;
        // 前脚を持ち上げる
        p.legFoot[0] = glm::vec3(1.4f, 0.8f * u, 2.2f);
    } else if (phase < 0.75f) {
        float u = (phase - 0.45f) / 0.30f;
        float sweepT = std::sin(u * glm::pi<float>() * 0.5f);
        p.bodyY = 1.05f;
        p.bodyYaw = glm::mix(-0.38f, 0.45f, sweepT);
        p.bodyPitch = 0.15f;
        p.bodyZ = glm::mix(-0.20f, 0.40f, sweepT);

        float sweepAngle = glm::mix(-0.4f, 1.9f, sweepT);
        float sweepRadius = 3.6f;
        p.legFoot[0] = glm::vec3(
            std::cos(sweepAngle) * sweepRadius,
            0.03f,
            std::sin(sweepAngle) * sweepRadius * 0.6f + 1.2f);
    } else {
        float u = (phase - 0.75f) / 0.25f;
        p.bodyY = glm::mix(1.05f, 1.10f, u);
        p.bodyYaw = glm::mix(0.45f, 0.0f, u);
        p.bodyPitch = glm::mix(0.15f, 0.0f, u);
        p.bodyZ = glm::mix(0.40f, 0.0f, u);
        p.legFoot[0] = glm::mix(p.legFoot[0], FOOT_ANCHORS[0], u);
    }
    return p;
}

// ============================================================
// main
// ============================================================
int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow("Grasshopper - Predator Attack",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) return -1;

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) return -1;
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return -1;

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    glEnable(GL_DEPTH_TEST);
    // 翅の透明度を使うため
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---- Shader ----
    std::string vCode = loadShaderFromFile("vertex.glsl");
    std::string fCode = loadShaderFromFile("fragment.glsl");
    if (vCode.empty() || fCode.empty()) return -1;

    auto compile = [](GLenum type, const char* src) {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, NULL);
        glCompileShader(s);
        GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) { char log[2048]; glGetShaderInfoLog(s, 2048, NULL, log); std::cerr << log << std::endl; }
        return s;
    };
    GLuint vs = compile(GL_VERTEX_SHADER, vCode.c_str());
    GLuint fs = compile(GL_FRAGMENT_SHADER, fCode.c_str());
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs); glLinkProgram(prog);

    // ---- Mesh buffers (dynamic, rebuilt each frame) ----
    GLuint VAO, VBO, EBO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 4096 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 8192 * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));   glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, partID));   glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, legID));    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, legPartID));glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pivotRoot));glEnableVertexAttribArray(5);
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pivotKnee));glEnableVertexAttribArray(6);

    // Instance (1個だけ使用、将来の複数対応用)
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    AntInstance inst = { glm::vec3(0), 0.0f, 0.0f, 1.0f };
    glBufferData(GL_ARRAY_BUFFER, sizeof(AntInstance), &inst, GL_STATIC_DRAW);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, position)); glEnableVertexAttribArray(7); glVertexAttribDivisor(7, 1);
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, rotation)); glEnableVertexAttribArray(8); glVertexAttribDivisor(8, 1);
    glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, phase));    glEnableVertexAttribArray(9); glVertexAttribDivisor(9, 1);
    glVertexAttribPointer(10,1, GL_FLOAT, GL_FALSE, sizeof(AntInstance), (void*)offsetof(AntInstance, speed));    glEnableVertexAttribArray(10); glVertexAttribDivisor(10, 1);

    Camera camera;
    camera.distance = 22.0f;
    camera.target   = glm::vec3(0, 1.2f, 0.0f);

    GLint uTimeLoc        = glGetUniformLocation(prog, "uTime");
    GLint uMotionSpeedLoc = glGetUniformLocation(prog, "uMotionSpeed");
    GLint uViewProjLoc    = glGetUniformLocation(prog, "uViewProj");
    GLint uColorLoc       = glGetUniformLocation(prog, "uColor");

    bool running = true;
    AttackMode mode = AttackMode::RUSH;
    float animPhase = 0.0f;
    bool isPlaying = true;
    Uint32 lastTime = SDL_GetTicks();
    float totalTime = 0.0f;
    int frameCount = 0;
    Uint32 fpsTimer = lastTime;

    std::vector<Vertex> verts;
    std::vector<uint32_t> inds;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (event.key.keysym.sym == SDLK_SPACE) isPlaying = !isPlaying;
                if (event.key.keysym.sym == SDLK_1) { mode = AttackMode::RUSH;  animPhase = 0.0f; }
                if (event.key.keysym.sym == SDLK_2) { mode = AttackMode::SWEEP; animPhase = 0.0f; }
                if (event.key.keysym.sym == SDLK_s) { mode = AttackMode::SWEEP; animPhase = 0.0f; }
                if (event.key.keysym.sym == SDLK_r) { mode = AttackMode::RUSH;  animPhase = 0.0f; }
                if (event.key.keysym.sym == SDLK_RIGHT) { animPhase = std::fmod(animPhase + 0.05f, 1.0f); }
                if (event.key.keysym.sym == SDLK_LEFT)  { animPhase = std::fmod(animPhase + 0.95f, 1.0f); }
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
                camera.distance = glm::clamp(camera.distance, 3.0f, 80.0f);
            }
        }

        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTime) / 1000.0f;
        lastTime = now;
        totalTime += dt;

        if (isPlaying) {
            animPhase = std::fmod(animPhase + dt * 0.35f, 1.0f);
        }

        // ---- Pose -> Mesh ----
        GrasshopperPose pose = (mode == AttackMode::RUSH)
                              ? computeRushPose(animPhase)
                              : computeSweepPose(animPhase);
        buildGrasshopperMesh(pose, verts, inds);

        // ---- Upload ----
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(Vertex), verts.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, inds.size() * sizeof(uint32_t), inds.data());

        // ---- Render ----
        glClearColor(0.10f, 0.20f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 proj = glm::perspective(glm::radians(40.0f), 1024.0f/768.0f, 0.1f, 200.0f);
        glm::mat4 view = camera.getView();
        glm::mat4 viewProj = proj * view;

        glUseProgram(prog);
        glUniform1f(uTimeLoc, totalTime);
        glUniform1f(uMotionSpeedLoc, 0.0f);  // 全アニメを停止 (meshが既にanimated)
        glUniformMatrix4fv(uViewProjLoc, 1, GL_FALSE, glm::value_ptr(viewProj));
        glUniform3f(uColorLoc, 0.28f, 0.62f, 0.20f);  // 明るい緑

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)inds.size(), GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);

        frameCount++;
        if (now - fpsTimer >= 1000) {
            const char* modeName = (mode == AttackMode::RUSH) ? "RUSH" : "SWEEP";
            std::cout << "FPS: " << frameCount
                      << " | Mode: " << modeName
                      << " | Phase: " << (int)(animPhase * 100) << "%"
                      << " | Verts: " << verts.size() << std::endl;
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