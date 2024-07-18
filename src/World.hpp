#pragma once

#include <glad/glad.h>
#include <vector>
#include "Math.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

namespace KM {
    class World
    {
    public:
        World(int width, int height, int depth);
        int getBlockId(int x, int y, int z);
        void setBlockId(int x, int y, int z, int id);

        int getBlockId(BlockPos pos);
        void setBlockId(BlockPos pos, int id);
        bool isInBounds(BlockPos pos);
        float getBlockBrightness(BlockPos pos);

        int getWidth();
        int getHeight();
        int getDepth();

        bool isInBounds(int x, int y, int z);

        void recalculateLightDepths(int x0, int y0, int x1, int y1);
        float getBlockBrightness(int x, int y, int z);

        std::vector<Box> getCubes(Box box);
    
        std::optional<HitResult> raycast(glm::vec3 var1, glm::vec3 var2);
        std::optional<HitResult> tileRaycast(int x, int y, int z, glm::vec3 var4, glm::vec3 var5);

        void write(std::ofstream *out);
    private:
        int m_width;
        int m_height;
        int m_depth;
        std::vector<int> m_blocks;
        std::vector<int> m_lightDepths;
    };

    struct Chunk {
    public:
        KM::World *world;
        int x0 = 0;
        int y0 = 0;
        int z0 = 0;
        int x1 = 0;
        int y1 = 0;
        int z1 = 0;
        
        GLuint vao = -1;
        GLuint vbo = -1;
        GLuint ibo = -1;

        GLuint vaoLayer1 = -1;
        GLuint vboLayer1 = -1;
        GLuint iboLayer1 = -1;

        int lasIndexCount = 0;
        int indexCount = 0;

        int lasIndexCountLayer1 = 0;
        int indexCountLayer1 = 0;

        bool dirty = true;

        Chunk(KM::World *world, int x0, int y0, int z0);

        Chunk(const Chunk&)  = delete;
        Chunk& operator=(const Chunk&)  = delete;

        ~Chunk();

        void rebuildGeometry(int layer);

        void render();

        void render(int layer);

        bool IsInFrustum(Frustum &frustum);
        
        static int chunkUpdates;
    };

}