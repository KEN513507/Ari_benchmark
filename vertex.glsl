#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in float aPartID;    // 0=Body, 1=Leg, 2=Antenna, 3=Wing
layout (location = 3) in float aLegID;
layout (location = 4) in float aLegPartID;
layout (location = 5) in vec3 aPivotRoot;
layout (location = 6) in vec3 aPivotKnee;

layout (location = 7) in vec3 aInstancePos;
layout (location = 8) in float aInstanceRot;
layout (location = 9) in float aInstancePhase;
layout (location = 10) in float aInstanceSpeed;

uniform mat4 uViewProj;
uniform float uTime;
uniform float uMotionSpeed;

out vec3 Normal;
out float vPartID;

const float PI = 3.14159265359;

mat3 rotateY(float angle) {
    float c = cos(angle); float s = sin(angle);
    return mat3(c, 0.0, s, 0.0, 1.0, 0.0, -s, 0.0, c);
}
mat3 rotateX(float angle) {
    float c = cos(angle); float s = sin(angle);
    return mat3(1.0, 0.0, 0.0, 0.0, c, -s, 0.0, s, c);
}

void main() {
    vec3 pos = aPos;
    vec3 norm = aNormal;

    float animPhase = uTime * aInstanceSpeed * uMotionSpeed + aInstancePhase;

    if (aPartID < 0.5) {
        // 胴体
        float bob = sin(animPhase * 2.0) * 0.015 * uMotionSpeed;
        pos.y += bob;
        float pitch = sin(animPhase) * 0.02 * uMotionSpeed;
        pos = rotateY(pitch) * pos;
    }
    else if (aPartID > 1.5 && aPartID < 2.5) {
        // 触角
        float sideSign = (aLegID < 0.5) ? 1.0 : -1.0;
        float t = uTime * 1.6 * uMotionSpeed;
        float forward  = 0.15 * uMotionSpeed;
        // 左右対称のため yaw 全体を sideSign で反転、nod は同位相にする
        float sway   = sin(t + 1.2) * 0.45 + forward;
        float sweepY = sideSign * sway;
        float nodX   = sin(t * 1.7 + 1.0) * 0.25;
        vec3 localPos = pos - aPivotRoot;
        float cs = cos(sweepY), ss = sin(sweepY);
        vec3 p1;
        p1.x = localPos.x * cs - localPos.z * ss;
        p1.y = localPos.y;
        p1.z = localPos.x * ss + localPos.z * cs;
        float cx = cos(nodX), sx = sin(nodX);
        vec3 p2;
        p2.x = p1.x;
        p2.y = p1.y * cx - p1.z * sx;
        p2.z = p1.y * sx + p1.z * cx;
        pos = aPivotRoot + p2;
        norm = rotateY(sweepY) * norm;
        norm = rotateX(nodX) * norm;
    }
    else if (aPartID > 2.5) {
        // 翅 - 静止
    }
    else {
        // 脚
        int legIdx = int(aLegID + 0.5);
        bool isGroupA = (legIdx == 0 || legIdx == 4 || legIdx == 2);
        float legPhase = isGroupA ? animPhase : animPhase + PI;

        // キリギリスの後脚 (legID=2,5) は跳躍姿勢のまま静止
        bool isHindLeg = (legIdx == 2 || legIdx == 5);
        if (isHindLeg) {
            // 静止
        } else {
            float swing = sin(legPhase) * 0.35 * uMotionSpeed;
            float lift  = max(0.0, sin(legPhase)) * 0.05 * uMotionSpeed;

            vec3 localPos = pos - aPivotRoot;
            float sideSign = (aPivotRoot.x > 0.0) ? 1.0 : -1.0;
            localPos = rotateY(swing * sideSign) * localPos;
            localPos = rotateX(-lift * 2.0 * sideSign) * localPos;
            localPos.y += lift;
            pos = localPos + aPivotRoot;

            pos.y += sin(animPhase * 2.0) * 0.015 * uMotionSpeed;
        }
    }

    mat3 worldRot = rotateY(aInstanceRot);
    pos = worldRot * pos;
    norm = worldRot * norm;
    pos += aInstancePos;

    Normal = norm;
    vPartID = aPartID;
    gl_Position = uViewProj * vec4(pos, 1.0);
}
