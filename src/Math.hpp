#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>

namespace KM {
    struct Box {
        float minX;
        float minY;
        float minZ;
        float maxX;
        float maxY;
        float maxZ;

        Box expand(float xa, float ya, float za);
        Box grow(float xa, float ya, float za);
        bool intersects(Box box);
        void move(float xa, float ya, float za);
        float clipXCollide(Box c, float xa);
        float clipYCollide(Box c, float ya);
        float clipZCollide(Box c, float za);
    };

    struct HitResult {
        int x;
        int y;
        int z;
        int face;
    };

    std::optional<glm::vec3> vec3ClipX(glm::vec3 vec, glm::vec3 other, float r);

    std::optional<glm::vec3> vec3ClipY(glm::vec3 vec, glm::vec3 other, float r);

    std::optional<glm::vec3> vec3ClipZ(glm::vec3 vec, glm::vec3 other, float r);

    struct Vertex {
        float x;
        float y;
        float z;
        float r;
        float g;
        float b;
        float a;
        float u;
        float v;
    };
    
    struct Vertex3FColor4F {
        float x;
        float y;
        float z;
        float r;
        float g;
        float b;
        float a;
    };

    struct BlockPos {
        int x;
        int y;
        int z;

        BlockPos(int x, int y, int z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        BlockPos Down();
        BlockPos Up();
        BlockPos North();
        BlockPos South();
        BlockPos West();
        BlockPos East();
    };

    // Credits: JOML
    struct FrustumIntersection
    {
        float nxX, nxY, nxZ, nxW;
        float pxX, pxY, pxZ, pxW;
        float nyX, nyY, nyZ, nyW;
        float pyX, pyY, pyZ, pyW;
        float nzX, nzY, nzZ, nzW;
        float pzX, pzY, pzZ, pzW;

        glm::vec4 planes[6];

        void set(glm::mat4 m) {
            nxX = m[0].w + m[0].x;
            nxY = m[1].w + m[1].x;
            nxZ = m[2].w + m[2].x;
            nxW = m[3].w + m[3].x;

            planes[0] = glm::vec4(nxX, nxY, nxZ, nxW);
            
            pxX = m[0].w + m[0].x;
            pxY = m[1].w + m[1].x;
            pxZ = m[2].w + m[2].x;
            pxW = m[3].w + m[3].x;

            planes[1] = glm::vec4(pxX, pxY, pxZ, pxW);

            nyX = m[0].w + m[0].y;
            nyY = m[1].w + m[1].y;
            nyZ = m[2].w + m[2].y;
            nyW = m[3].w + m[3].y;

            planes[2] = glm::vec4(nyX, nyY, nyZ, nyW);

            pyX = m[0].w + m[0].y;
            pyY = m[1].w + m[1].y;
            pyZ = m[2].w + m[2].y;
            pyW = m[3].w + m[3].y;

            planes[3] = glm::vec4(pyX, pyY, pyZ, pyW);

            nzX = m[0].w + m[0].z;
            nzY = m[1].w + m[1].z;
            nzZ = m[2].w + m[2].z;
            nzW = m[3].w + m[3].z;

            planes[4] = glm::vec4(nzX, nzY, nzZ, nzW);

            pzX = m[0].w + m[0].z;
            pzY = m[1].w + m[1].z;
            pzZ = m[2].w + m[2].z;
            pzW = m[3].w + m[3].z;

            planes[5] = glm::vec4(pzX, pzY, pzZ, pzW);
        }

        bool TestAab(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
        {
            return nxX * (nxX < 0 ? minX : maxX) + nxY * (nxY < 0 ? minY : maxY) + nxZ * (nxZ < 0 ? minZ : maxZ) >= -nxW &&
               pxX * (pxX < 0 ? minX : maxX) + pxY * (pxY < 0 ? minY : maxY) + pxZ * (pxZ < 0 ? minZ : maxZ) >= -pxW &&
               nyX * (nyX < 0 ? minX : maxX) + nyY * (nyY < 0 ? minY : maxY) + nyZ * (nyZ < 0 ? minZ : maxZ) >= -nyW &&
               pyX * (pyX < 0 ? minX : maxX) + pyY * (pyY < 0 ? minY : maxY) + pyZ * (pyZ < 0 ? minZ : maxZ) >= -pyW &&
               nzX * (nzX < 0 ? minX : maxX) + nzY * (nzY < 0 ? minY : maxY) + nzZ * (nzZ < 0 ? minZ : maxZ) >= -nzW &&
               pzX * (pzX < 0 ? minX : maxX) + pzY * (pzY < 0 ? minY : maxY) + pzZ * (pzZ < 0 ? minZ : maxZ) >= -pzW;
        }
    };

    struct Frustum
    {
        FrustumIntersection frustumIntersection;
        float x = 0;
        float y = 0;
        float z = 0;

        void Init(glm::mat4 positionMatrix, glm::mat4 projectionMatrix) {
            this->frustumIntersection.set(positionMatrix * projectionMatrix);
        }

        void SetPosition(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        bool IsVisible(Box box) {
            return true;
            // float x0 = (float)(box.minX - this->x);
            // float y0 = (float)(box.minY - this->y);
            // float z0 = (float)(box.minZ - this->z);
            // float x1 = (float)(box.maxX - this->x);
            // float y1 = (float)(box.maxY - this->y);
            // float z1 = (float)(box.maxZ - this->z);
            // return this->frustumIntersection.TestAab(x0, y0, z0, x1, y1, z1);
        }
    };
}