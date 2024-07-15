#include "Math.hpp"

namespace KM {
    Box Box::expand(float xa, float ya, float za)
    {
        float minX = this->minX;
        float minY = this->minY;
        float minZ = this->minZ;
        float maxX = this->maxX;
        float maxY = this->maxY;
        float maxZ = this->maxZ;

        if (xa < 0.0F) minX += xa;
        if (xa > 0.0F) maxX += xa;

        if (ya < 0.0F) minY += ya;
        if (ya > 0.0F) maxY += ya;

        if (za < 0.0F) minZ += za;
        if (za > 0.0F) maxZ += za;

        return { minX, minY, minZ, maxX, maxY, maxZ };
    }

    Box Box::grow(float xa, float ya, float za) {
        return { this->minX - xa, this->minY - ya, this->minZ - za, this->maxX + xa, this->maxY + ya, this->maxZ + za }; 
    }

    bool Box::intersects(Box box) {
        return box.maxX > this->minX && box.minX < this->maxX ? (box.maxY > this->minY && box.minY < this->maxY ? box.maxZ > this->minZ && box.minZ < this->maxZ : false) : false;
    }

    void Box::move(float xa, float ya, float za) {
        this->minX += xa;
        this->minY += ya;
        this->minZ += za;
        this->maxX += xa;
        this->maxY += ya;
        this->maxZ += za;
    }

    float Box::clipXCollide(Box c, float xa) {
        if (c.maxY > this->minY && c.minY < this->maxY) {
            if (c.maxZ > this->minZ && c.minZ < this->maxZ) {
                float max;
                if (xa > 0.0F && c.maxX <= this->minX) {
                    max = this->minX - c.maxX;
                    if (max < xa) {
                        xa = max;
                    }
                }

                if (xa < 0.0F && c.minX >= this->maxX) {
                    max = this->maxX - c.minX;
                    if (max > xa) {
                        xa = max;
                    }
                }

                return xa;
            }
            return xa;
        }
        return xa;
    }

    float Box::clipYCollide(Box c, float ya) {
        if (c.maxX > this->minX && c.minX < this->maxX) {
            if (c.maxZ > this->minZ && c.minZ < this->maxZ) {
                float max;
                if (ya > 0.0F && c.maxY <= this->minY) {
                    max = this->minY - c.maxY;
                    if (max < ya) {
                        ya = max;
                    }
                }

                if (ya < 0.0F && c.minY >= this->maxY) {
                    max = this->maxY - c.minY;
                    if (max > ya) {
                        ya = max;
                    }
                }

                return ya;
            }
            return ya;
        }
        return ya;
    }

    float Box::clipZCollide(Box c, float za) {
        if(c.maxX > this->minX && c.minX < this->maxX) {
            if(c.maxY > this->minY && c.minY < this->maxY) {
                float max;
                if(za > 0.0F && c.maxZ <= this->minZ) {
                    max = this->minZ - c.maxZ;
                    if(max < za) {
                        za = max;
                    }
                }

                if(za < 0.0F && c.minZ >= this->maxZ) {
                    max = this->maxZ - c.minZ;
                    if(max > za) {
                        za = max;
                    }
                }

                return za;
            }
            return za;
        }
        return za;
    }

    std::optional<glm::vec3> vec3ClipX(glm::vec3 vec, glm::vec3 other, float r) {
        float a = other.x - vec.x;
        float b = other.y - vec.y;
        float c = other.z - vec.z;

        if (a * a < 1.0E-7f) {
			return std::nullopt;
		} else {
			r = (r - vec.x) / a;
			return r >= 0.0f && r <= 1.0f ? std::optional<glm::vec3>{{vec.x + a * r, vec.y + b * r, vec.z + c * r}} : std::nullopt;
		}
    }

    std::optional<glm::vec3> vec3ClipY(glm::vec3 vec, glm::vec3 other, float r) {
        float a = other.x - vec.x;
        float b = other.y - vec.y;
        float c = other.z - vec.z;

        if (b * b < 1.0E-7f) {
			return std::nullopt;
		} else {
			r = (r - vec.y) / b;
			return r >= 0.0f && r <= 1.0f ? std::optional<glm::vec3>{{vec.x + a * r, vec.y + b * r, vec.z + c * r}} : std::nullopt;
		}
    }

    std::optional<glm::vec3> vec3ClipZ(glm::vec3 vec, glm::vec3 other, float r) {
        float a = other.x - vec.x;
        float b = other.y - vec.y;
        float c = other.z - vec.z;

        if (c * c < 1.0E-7f) {
			return std::nullopt;
		} else {
			r = (r - vec.z) / c;
			return r >= 0.0f && r <= 1.0f ? std::optional<glm::vec3>{{vec.x + a * r, vec.y + b * r, vec.z + c * r}} : std::nullopt;
		}
    }

    BlockPos BlockPos::Down() {
        return BlockPos(this->x, this->y - 1, this->z);
    }

    BlockPos BlockPos::Up() {
        return BlockPos(this->x, this->y + 1, this->z);
    }

    BlockPos BlockPos::North() {
        return BlockPos(this->x, this->y, this->z - 1);
    }

    BlockPos BlockPos::South() {
        return BlockPos(this->x, this->y, this->z + 1);
    }

    BlockPos BlockPos::West() {
        return BlockPos(this->x - 1, this->y, this->z);
    }

    BlockPos BlockPos::East() {
        return BlockPos(this->x + 1, this->y, this->z);
    }
}