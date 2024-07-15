#pragma once

#include <glm/glm.hpp>
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
}