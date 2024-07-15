#include <glm/gtc/type_ptr.hpp>
#include "Player.hpp"

namespace KM {
    Player::Player() {
        this->yaw = 135;
        this->setPosition(0, 64 / 3 + 2, 0);
    }

    void Player::setWorld(World *world) {
        this->world = world;
    }

    void Player::setPosition(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->bb = { this->x - this->w, this->y - this->h, this->z - w, this->x + w, this->y + h, this->z + w };
    }

    void Player::turn(float xo, float yo) {
        this->yaw = (float)((double)this->yaw + (double)xo * 0.15);
        this->pitch = (float)((double)this->pitch - (double)yo * 0.15);
        if(this->pitch < -90.0F) {
            this->pitch = -90.0F;
        }

        if(this->pitch > 90.0F) {
            this->pitch = 90.0F;
        }
    }

    void Player::tick() {
    }

    // Credits: MCClassic
    void Player::move(float xa, float ya, float za) {
        float xaOrg = xa;
        float yaOrg = ya;
        float zaOrg = za;
        auto aABBs = this->world->getCubes(this->bb.expand(xa, ya, za));

        int i;
        for (i = 0; i < aABBs.size(); ++i) {
            ya = aABBs[i].clipYCollide(this->bb, ya);
        }

        this->bb.move(0.0F, ya, 0.0F);

        for (i = 0; i < aABBs.size(); ++i) {
            xa = aABBs[i].clipXCollide(this->bb, xa);
        }

        this->bb.move(xa, 0.0F, 0.0F);

        for (i = 0; i < aABBs.size(); ++i) {
            za = aABBs[i].clipZCollide(this->bb, za);
        }

        this->bb.move(0.0F, 0.0F, za);
        this->onGround = yaOrg != ya && yaOrg < 0.0F;
        if(xaOrg != xa) {
            this->xd = 0.0F;
        }

        if(yaOrg != ya) {
            this->yd = 0.0F;
        }

        if(zaOrg != za) {
            this->zd = 0.0F;
        }

        this->x = (this->bb.minX + this->bb.maxX) / 2.0F;
        this->y = this->bb.minY + 1.62F;
        this->z = (this->bb.minZ + this->bb.maxZ) / 2.0F;
    }

    // Credits: MCClassic
    void Player::mouseRelative(float xa, float za, float speed) {
        float dist = xa * xa + za * za;
        if (dist >= 0.01F) {
            dist = speed / (float)glm::sqrt((double)dist);
            xa *= dist;
            za *= dist;
            float sin = (float)glm::sin(((double)this->yaw * glm::pi<double>() / 180.0));
            float cos = (float)glm::cos((double)this->yaw * glm::pi<double>() / 180.0);
            this->xd += xa * cos - za * sin;
            this->zd += za * cos + xa * sin;
        }
    }
}