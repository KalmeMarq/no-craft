#include "World.hpp"
#include "Math.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Application.hpp"
#include <cstdlib>
#include <glad/glad.h>
#include <time.h> 
#include <glm/glm.hpp>

namespace KM {
    void placeOakTree(KM::World *world, int x, int y, int z) {
        world->setBlockId(x, y, z, 9);
        world->setBlockId(x, y + 1, z, 9);
        world->setBlockId(x, y + 2, z, 9);
        world->setBlockId(x, y + 3, z, 9);
        world->setBlockId(x, y + 4, z, 9);
        world->setBlockId(x, y + 5, z, 9);

        world->setBlockId(x, y + 6, z, 7);
        
        for (int xx = -2; xx < 3; ++xx) {
            for (int zz = -2; zz < 3; ++zz) {
                if (xx == 0 && zz == 0) continue;

                world->setBlockId(x + xx, y + 3, z + zz, 7);
                world->setBlockId(x + xx, y + 4, z + zz, 7);
            }
        }

        world->setBlockId(x - 1, y + 5, z, 7);
        world->setBlockId(x - 1, y + 6, z, 7);
        world->setBlockId(x + 1, y + 5, z, 7);
        world->setBlockId(x + 1, y + 6, z, 7);
        world->setBlockId(x, y + 5, z - 1, 7);
        world->setBlockId(x, y + 6, z - 1, 7);
        world->setBlockId(x, y + 5, z + 1, 7);
        world->setBlockId(x, y + 6, z + 1, 7);
    }

    void placeLake(KM::World *world, int x, int y, int z)
    {
        for (int yy = 0; yy < 3; ++yy)
        {
            for (int zz = 0; zz < 8; ++zz)
            {
                for (int xx = 0; xx < 8; ++xx)
                {
                    world->setBlockId(x + xx, y - yy, z + zz, 5);
                }
            }
        } 
    }

    World::World(int width, int height, int depth) : m_width(width), m_height(height), m_depth(depth)
    {
        this->m_blocks.reserve(this->m_width * this->m_height * this->m_depth);
        this->m_lightDepths.reserve(this->m_width * this->m_height);

        std::cout << "Generating World\n";
        for (int y = 0; y < this->m_depth; ++y) {
            int h = this->m_depth / 3;
            int blockId = y == 0 ? 6 : y > h ? 0 : y < h ? y < h - 1 ? 3 : 2 : 1;
            
            for (int z = 0; z < this->m_height; ++z) {
                for (int x = 0; x < this->m_width; ++x) {
                    this->m_blocks[(y * this->m_height + z) * this->m_width + x] = blockId;
                }
            }
        }

        srand(time(NULL));

        std::cout << "Placing Trees\n";
        for (int i = 0; i < 64; i++) {
            int x = (rand() % (this->m_width - 6)) + 3;
            int z = (rand() % (this->m_height - 6)) + 3;

            placeOakTree(this, x, this->m_depth / 3 + 1, z);
        }
        
        placeLake(this, 70, this->m_depth / 3, 70);

        this->recalculateLightDepths(0, 0, this->m_width, this->m_height);
    }

    int World::getBlockId(int x, int y, int z) {
        return x >= 0 && y >= 0 && z >= 0 && x < this->m_width && y < this->m_depth && z < this->m_height ? this->m_blocks[(y * this->m_height + z) * this->m_width + x] : 0;
    }

    int World::getBlockId(BlockPos pos) {
        return getBlockId(pos.x, pos.y, pos.z);
    }

    bool World::isInBounds(int x, int y, int z) {
        return x >= 0 && y >= 0 && z >= 0 && x < this->m_width && y < this->m_depth && z < this->m_height;
    }

    bool World::isInBounds(BlockPos pos) {
        return isInBounds(pos.x, pos.y, pos.z);
    }

    void World::setBlockId(int x, int y, int z, int id) {
        if (x >= 0 && y >= 0 && z >= 0 && x < this->m_width && y < this->m_depth && z < this->m_height) {
            this->m_blocks[(y * this->m_height + z) * this->m_width + x] = id;
        }
    }

    void World::setBlockId(BlockPos pos, int id) {
        setBlockId(pos.x, pos.y, pos.z, id);
    }

    int World::getWidth() {
        return this->m_width;
    }

    int World::getHeight() {
        return this->m_height;
    }

    int World::getDepth() {
        return this->m_depth;
    }

    void World::recalculateLightDepths(int x0, int y0, int x1, int y1)
    {
        for (int x = x0; x < x0 + x1; ++x) {
			for (int z = y0; z < y0 + y1; ++z) {
				int oldDepth = this->m_lightDepths[x + z * this->m_width];

				int y;
				for(y = this->m_depth - 1; y > 0 && this->getBlockId(x, y, z) == 0; --y) {
				}

				this->m_lightDepths[x + z * this->m_width] = y;
			}
		}
    }

    float World::getBlockBrightness(int x, int y, int z)
    {
        float dark = 0.8F;
		float light = 1.0F;
        return x >= 0 && y >= 0 && z >= 0 && x < this->m_width && y < this->m_depth && z < this->m_height ? (y < this->m_lightDepths[x + z * this->m_width] ? dark : light) : light;
    }

    
    float World::getBlockBrightness(BlockPos pos)
    {
        return getBlockBrightness(pos.x, pos.y, pos.z);
    }

    std::vector<Box> World::getCubes(Box box)
    {
        std::vector<Box> cubeBoxes;

        int x0 = glm::clamp((int) box.minX, 0, this->m_width);
		int x1 = glm::clamp((int) (box.maxX + 1.0F), 0, this->m_width);
		int y0 = glm::clamp((int) box.minY, 0, this->m_width);
		int y1 = glm::clamp((int) (box.maxY + 1.0F), 0, this->m_width);
		int z0 = glm::clamp((int) box.minZ, 0, this->m_width);
		int z1 = glm::clamp((int) (box.maxZ + 1.0F), 0, this->m_width);

        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                for (int x = x0; x < x1; ++x) {
					int blockId = this->getBlockId(x, y, z);
                    if (blockId != 0 && blockId != 5) {
						cubeBoxes.push_back({ (float)x, (float)y, (float)z, (float)(x + 1), (float)(y + 1), (float)(z + 1) });
					}
				}
			}
		}

        return cubeBoxes;
    }

    // Credits: MCClassic
    std::optional<HitResult> World::raycast(glm::vec3 start, glm::vec3 end)
    {
        int endX = (int) glm::floor((double) end.x);
        int endY = (int) glm::floor((double) end.y);
        int endZ = (int) glm::floor((double) end.z);
        int x = (int) glm::floor((double) start.x);
        int y = (int) glm::floor((double) start.y);
        int z = (int) glm::floor((double) start.z);
        
        int limit = 20;
        while (limit-- >= 0) {
            if (x == endX && y == endY && z == endZ) {
                return std::nullopt;
            }

            float var10 = 999.0F;
            float var11 = 999.0F;
            float var12 = 999.0F;
            if (endX > x) {
                var10 = (float)x + 1.0F;
            }

            if (endX < x) {
                var10 = (float)x;
            }

            if (endY > y) {
                var11 = (float)y + 1.0F;
            }

            if (endY < y) {
                var11 = (float)y;
            }

            if (endZ > z) {
                var12 = (float)z + 1.0F;
            }

            if (endZ < z) {
                var12 = (float)z;
            }

            float var13 = 999.0F;
            float var14 = 999.0F;
            float var15 = 999.0F;
            float var16 = end.x - start.x;
            float var17 = end.y - start.y;
            float var18 = end.z - start.z;
            if (var10 != 999.0F) {
                var13 = (var10 - start.x) / var16;
            }

            if (var11 != 999.0F) {
                var14 = (var11 - start.y) / var17;
            }

            if (var12 != 999.0F) {
                var15 = (var12 - start.z) / var18;
            }

            bool var19 = false;
            int var24;
            if (var13 < var14 && var13 < var15) {
                if (endX > x) {
                    var24 = 4;
                } else {
                    var24 = 5;
                }

                start.x = var10;
                start.y += var17 * var13;
                start.z += var18 * var13;
            } else if (var14 < var15) {
                if (endY > y) {
                    var24 = 0;
                } else {
                    var24 = 1;
                }

                start.x += var16 * var14;
                start.y = var11;
                start.z += var18 * var14;
            } else {
                if (endZ > z) {
                    var24 = 2;
                } else {
                    var24 = 3;
                }

                start.x += var16 * var15;
                start.y += var17 * var15;
                start.z = var12;
            }

            glm::vec3 var20 = glm::vec3(start.x, start.y, start.z);
            x = (int)(var20.x = (float)glm::floor((double)start.x));
            if (var24 == 5) {
                --x;
                ++var20.x;
            }

            y = (int)(var20.y = (float)glm::floor((double)start.y));
            if (var24 == 1) {
                --y;
                ++var20.y;
            }

            z = (int)(var20.z = (float)glm::floor((double)start.z));
            if (var24 == 3) {
                --z;
                ++var20.z;
            }

            int blockId = this->getBlockId(x, y, z);
            if (blockId > 0 && blockId != 5) {
                std::optional<HitResult> result =  this->tileRaycast(x, y, z, start, end);
                if (result.has_value()) {
                    return result;
                }
            }
        }
     
        return std::nullopt;
    }

    std::optional<HitResult> World::tileRaycast(int x, int y, int z, glm::vec3 start, glm::vec3 end)
    {
        start += glm::vec3((float)-x, (float)-y, (float)-z);
        end += glm::vec3((float)-x, (float)-y, (float)-z);
        std::optional<glm::vec3> minX = vec3ClipX(start, end, 0);
        std::optional<glm::vec3> maxX = vec3ClipX(start, end, 1);
        std::optional<glm::vec3> minY = vec3ClipY(start, end, 0);
        std::optional<glm::vec3> maxY = vec3ClipY(start, end, 1);
        std::optional<glm::vec3> minZ = vec3ClipZ(start, end, 0);
        std::optional<glm::vec3> maxZ = vec3ClipZ(start, end, 1);

		std::optional<glm::vec3> hit = std::nullopt;

		if (minX.has_value()) {
			hit = minX;
		}

		if (maxX.has_value() && (!hit.has_value() || glm::distance(start, maxX.value()) < glm::distance(start, hit.value()))) {
			hit = maxX;
		}

		if (minY.has_value() && (!hit.has_value() || glm::distance(start, minY.value()) < glm::distance(start, hit.value()))) {
			hit = minY;
		}

		if (maxY.has_value() && (!hit.has_value() || glm::distance(start, maxY.value()) < glm::distance(start, hit.value()))) {
			hit = maxY;
		}

		if (minZ.has_value() && (!hit.has_value() || glm::distance(start, minZ.value()) < glm::distance(start, hit.value()))) {
			hit = minZ;
		}

		if (maxZ.has_value() && (!hit.has_value() || glm::distance(start, maxZ.value()) < glm::distance(start, hit.value()))) {
			hit = maxZ;
		}

		if (!hit.has_value()) {
			return std::nullopt;
		} else {
			int face = -1;

			if (hit == minX) {
				face = 4;
			}

			if (hit == maxX) {
				face = 5;
			}

			if (hit == minY) {
				face = 0;
			}

			if (hit == maxY) {
				face = 1;
			}

			if (hit == minZ) {
				face = 2;
			}

			if (hit == maxZ) {
				face = 3;
			}

			return std::optional<HitResult>{{ x, y, z, face }};
		}
    }

    void World::write(std::ofstream *out)
    {
        int *data = this->m_blocks.data();
        out->write(reinterpret_cast<char*>(data), sizeof(int) * this->m_width * this->m_height * this->m_depth);
    }

    Chunk::Chunk(World *world, int x0, int y0, int z0) {
        this->world = world;
        this->x0 = x0;
        this->y0 = y0;
        this->z0 = z0;
        this->x1 = x0 + 16;
        this->y1 = y0 + 64;
        this->z1 = z0 + 16;
        glGenVertexArrays(3, this->vao);
        glGenBuffers(3, this->vbo);
        glGenBuffers(3, this->ibo);
        this->dirty = true;
    }

    Chunk::~Chunk() {
        glDeleteVertexArrays(3, this->vao);
        glDeleteBuffers(3, this->vbo);
        glDeleteBuffers(3, this->ibo);
    }

    void Chunk::rebuildGeometry(int layer) {
        std::vector<KM::Vertex> vertices;

        for (int y = this->y0; y < this->y1; ++y) {
            for (int z = this->z0; z < this->z1; ++z) {
                for (int x = this->x0; x < this->x1; ++x) {
                    Application::RenderTile(vertices, this->world, BlockPos(x, y, z), this->world->getBlockId(x, y, z), layer);
                }
            }
        }

        int vertexCount = vertices.size();
        int indexCount = vertexCount / 4 * 6;

        glBindVertexArray(this->vao[layer]);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo[layer]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(KM::Vertex) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid *)((3 + 4) * sizeof(float)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo[layer]);

        if (this->lastIndexCount[layer] < indexCount) {
            std::vector<unsigned int> index;

            for (int i = 0; i < indexCount; ++i) {
                index.push_back(i * 4);
                index.push_back(i * 4 + 1);
                index.push_back(i * 4 + 2);
                index.push_back(i * 4 + 2);
                index.push_back(i * 4 + 3);
                index.push_back(i * 4);
            }

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * index.size(), &index[0], GL_DYNAMIC_DRAW);
            this->lastIndexCount[layer] = indexCount;
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        this->indexCount[layer] = indexCount;
    }

    void Chunk::render() {
        this->render(0);
    }

    void Chunk::render(int layer)
    {
        if (this->dirty) {
            chunkUpdates++;
            this->rebuildGeometry(0);
            this->rebuildGeometry(1);
            this->rebuildGeometry(2);
            this->dirty = false;
        }

        if (this->indexCount[layer] > 0) {
            glBindVertexArray(this->vao[layer]);
            glDrawElements(GL_TRIANGLES, this->indexCount[layer], GL_UNSIGNED_INT, 0);
        }
    }

    bool Chunk::IsInFrustum(Frustum &frustum)
    {
        return frustum.IsVisible({ (float) x0, (float) y0, (float) z0, (float) x1, (float) y1, (float) z1 });
    }

    int Chunk::chunkUpdates = 0;
}