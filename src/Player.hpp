#pragma once

#include "World.hpp"
#include "Math.hpp"

namespace KM {
    struct Player {
    public:
        KM::World *world;
        float w = 0.3f;
        float h = 0.9f;
        float xo = 0.f;
        float yo = 0.f;
        float zo = 0.f;
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
        float pitch = 0.f;
        float yaw = 0.f;
        float xd = 0.f;
        float zd = 0.f;
        float yd = 0.f;
        bool onGround = false;
        bool isInsideWater = false;
        KM::Box bb;

        Player();
        void setWorld(World *world);
        void setPosition(float x, float y, float z);
        void turn(float xo, float yo);
        void tick();
        void move(float xa, float ya, float za);
        void mouseRelative(float xa, float za, float speed);
    };
}