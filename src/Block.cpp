#include "Block.hpp"

namespace KM
{
    BlockDefinition Blocks[BLOCK_COUNT];

    void InitBlockDefs()
    {
        Blocks[0] = { { 0 }, 0, false, "Air" };
        Blocks[1] = { { 2, 0, 3, 3, 3, 3 }, 0, true, "Grass" }; // Grass
        Blocks[2] = { { 2, 2, 2, 2, 2, 2 }, 0, true, "Dirt" }; // Dirt
        Blocks[3] = { { 1, 1, 1, 1, 1, 1 }, 0, true, "Stone" }; // Stone
        Blocks[4] = { { 4, 4, 4, 4, 4, 4 }, 0, true, "Planks" }; // Planks
        Blocks[5] = { { 18, 18, 18, 18, 18, 18 }, 2, true, "Water" }; // Water
        Blocks[6] = { { 17, 17, 17, 17, 17, 17 }, 0, true, "Bedrock" }; // Bedrock
        Blocks[7] = { { 22, 22, 22, 22, 22, 22 }, 1, false, "Leaves" }; // Leaves
        Blocks[8] = { { 49, 49, 49, 49, 49, 49 }, 1, false, "Glass" }; // Glass
        Blocks[9] = { { 21, 21, 20, 20, 20, 20 }, 0, true, "Log" }; // Log

        Blocks[10] = { { 64, 64, 64, 64, 64, 64 }, 0, true, "Red Wool" }; // Red Wool
        Blocks[11] = { { 65, 65, 65, 65, 65, 65 }, 0, true, "Orange Wool" }; // Orange Wool
        Blocks[12] = { { 66, 66, 66, 66, 66, 66 }, 0, true, "Yellow Wool" }; // Yellow Wool
        Blocks[13] = { { 67, 67, 67, 67, 67, 67 }, 0, true, "White Wool" }; // White Wool
        Blocks[14] = { { 68, 68, 68, 68, 68, 68 }, 0, true, "Black Wool" }; // Black Wool
        Blocks[15] = { { 69, 69, 69, 69, 69, 69 }, 0, true, "Blue Wool" }; // Blue Wool
        
        Blocks[16] = { { 5, 5, 5, 5, 5, 5 }, 0, true, "Sand" }; // Sand
        Blocks[17] = { { 6, 6, 6, 6, 6, 6 }, 0, true, "Gravel" }; // Gravel
    }
}