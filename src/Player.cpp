#include <glm/gtc/type_ptr.hpp>
#include "Player.hpp"
#include "Application.hpp"
#include <glfw/glfw3.h>

namespace KM {
    Player::Player() {
        this->yaw = 135;
        this->setPosition(64, 64 / 3 + 2, 64);
    }

    void Player::setWorld(World* world) {
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
        this->xo = this->x;
        this->yo = this->y;
        this->zo = this->z;

        float xa = 0;
        float za = 0;

        Application* app = Application::GetInstance();
        
        if (glfwGetKey(app->window.GetHandle(), GLFW_KEY_SPACE) != GLFW_RELEASE && this->onGround) {
            this->yd = 0.12f;

            if (this->isInsideWater)
            {
                this->yd += 0.04f;
            }
        }

        if (glfwGetKey(app->window.GetHandle(), GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE || glfwGetKey(app->window.GetHandle(), GLFW_KEY_RIGHT_SHIFT) != GLFW_RELEASE && !this->onGround) {
            this->yd = -0.12f;
        }

        if (glfwGetKey(app->window.GetHandle(), GLFW_KEY_W) != GLFW_RELEASE) {
            za -= 1;
        }

        if (glfwGetKey(app->window.GetHandle(), GLFW_KEY_S) != GLFW_RELEASE) {
            za += 1;
        }

        if (glfwGetKey(app->window.GetHandle(), GLFW_KEY_A) != GLFW_RELEASE) {
            xa -= 1;
        }

        if (glfwGetKey(app->window.GetHandle(), GLFW_KEY_D) != GLFW_RELEASE) {
            xa += 1;
        }

        this->mouseRelative(xa, za, this->onGround ? 0.02f : 0.005f);
        this->yd -= 0.005f;
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.91f;
        this->yd *= 0.98f;
        this->zd *= 0.91f;

        this->isInsideWater = this->world->getBlockId(this->x, this->y, this->z) == 5;

        if (this->onGround) {
            this->xd *= 0.8F;
            this->zd *= 0.8F;
        }
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