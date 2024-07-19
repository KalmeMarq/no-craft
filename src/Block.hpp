#pragma once

#include <string>

#define BLOCK_COUNT 18

namespace KM
{
    struct BlockDefinition
    {
        int textureIndex[6];
        int renderLayer;
        bool useAO;
        std::string name;
    };

    extern BlockDefinition Blocks[BLOCK_COUNT];

    void InitBlockDefs();
}
