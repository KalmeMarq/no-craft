#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Application.hpp"

struct BlockDef {
    int guiTextureIndex;
    int textureIndex[6];
    int renderLayer;
};

#define BLOCK_COUNT 10
static BlockDef BLOCKS_DEFS[BLOCK_COUNT] = {
    { 0, { 0 }, 0 }, // Air
    { 3, { 2, 0, 3, 3, 3, 3 }, 0 }, // Grass
    { 2, { 2, 2, 2, 2, 2, 2 }, 0 }, // Dirt
    { 1, { 1, 1, 1, 1, 1, 1 }, 0 }, // Stone
    { 4, { 4, 4, 4, 4, 4, 4 }, 0 }, // Planks
    { 18, { 18, 18, 18, 18, 18, 18 }, 2 }, // Water
    { 17, { 17, 17, 17, 17, 17, 17 }, 0 }, // Bedrock
    { 22, { 22, 22, 22, 22, 22, 22 }, 1 }, // Leaves
    { 49, { 49, 49, 49, 49, 49, 49 }, 1 }, // Glass
    { 20, { 21, 21, 20, 20, 20, 20 }, 0 } // Log
};

int directionNormalVectors[6][3] = {
    { 0, -1, 0 },
    { 0, 1, 0 },
    { 0, 0, -1 },
    { 0, 0, 1 },
    { -1, 0, 0 },
    { 1, 0, 0 }
};

static int chunkRendered = 0;
static int chunkTotal = 0;

namespace KM {
    Application* Application::appInstance { nullptr };

    Application::Application() : m_menu(nullptr)
    {
        appInstance = this;
    }

    Application::~Application()
    {
        this->SetMenu(nullptr);

        std::cout << "Clearing Chunks\n";
        for (KM::Chunk* chunk : m_chunks) {
            delete chunk;
        }
        m_chunks.clear();
        std::cout << "Closing...\n";
    }

    void Application::Run()
    {
        std::cout << "Creating Window\n";
        if (!window.Init(854, 480, "NoCraft", this)) {
            std::cout << "Failed to create Window\n";
            return;
        }
        window.SetVsync(false);

        renderer.Init();

        std::cout << "Creating Chunks\n";
        // this->StartWorld();

        std::cout << "Build Selection Box Geometry\n";

        glGenVertexArrays(1, &m_selectionBoxVao);
        glBindVertexArray(m_selectionBoxVao);

        GLuint selectionBoxVbo;
        glGenBuffers(1, &selectionBoxVbo);
        glBindBuffer(GL_ARRAY_BUFFER, selectionBoxVbo);

        std::vector<KM::Vertex3FColor4F> verts;
        {
            KM::HitResult rh = { 0, 0, 0, 0 };
            RenderSelectionBox(verts, rh);
            glBufferData(GL_ARRAY_BUFFER, sizeof(KM::Vertex3FColor4F) * verts.size(), &verts[0], GL_DYNAMIC_DRAW);
        }
        m_selectionVertexCount = verts.size();

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (GLvoid *)(3 * sizeof(float)));
        
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        std::cout << "Loaded\n";

        double lastTime = glfwGetTime();
        int frameCounter = 0;
        int tickCounter = 0;

        double tickLastTime = glfwGetTime();
        double tickTimeBehind = 0.0;
        double tickTargetTimeStep = 1.0 / 60.0;

        this->SetMenu(new TitleMenu());

        while (this->m_running)
        {
            if (window.ShouldClose()) Shutdown();

            // https://gamedev.stackexchange.com/questions/73700/make-opengl-program-only-update-every-1-60-seconds   
            tickTimeBehind += glfwGetTime() - tickLastTime;
            tickLastTime = glfwGetTime();

            while(tickTimeBehind >= tickTargetTimeStep)
            {
                this->Tick();
                tickCounter++;
                tickTimeBehind -= tickTargetTimeStep;
            }

            this->Render();

            window.Update();
            frameCounter++;
            chunkRendered = 0;
            chunkTotal = 0;

            while (glfwGetTime() - lastTime > 1.0) {
                lastTime += 1.0;
                m_fpsString = std::to_string(frameCounter) + " FPS " + std::to_string(tickCounter) + " TPS";
                KM::Chunk::chunkUpdates = 0;
                frameCounter = 0;
                tickCounter = 0;
            }
        }

        glDeleteBuffers(1, &selectionBoxVbo);
        glDeleteVertexArrays(1, &m_selectionBoxVao);

        renderer.Close();
    }

    void Application::OnResize()
    {
        std::cout << "Viewport Resized";
        glViewport(0, 0, this->window.GetWidth(), this->window.GetHeight());

        if (this->m_menu != nullptr)
        {
            int scale = CalculateGuiScale(this->window.GetWidth(), this->window.GetHeight());
            this->m_menu->Resize(this->window.GetWidth() / scale, this->window.GetHeight() / scale);
        }
    }

    void Application::OnKey(int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_B && action == GLFW_RELEASE) {
            this->SetMenu(new InventoryMenu());
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
            if (this->m_mouseGrabbed) {
                glfwSetInputMode(this->window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetCursorPos(this->window.GetHandle(), this->window.GetWidth() / 2, this->window.GetHeight() / 2);
                this->m_mouseGrabbed = false;
                this->SetMenu(new GameMenu());
            }
        }

        if (action == GLFW_RELEASE) {
            if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
                this->m_selectedItem = key - GLFW_KEY_1;
            }

            if (key == GLFW_KEY_F3) {
                this->m_showDebugInfo = !this->m_showDebugInfo;
            }
        }
    }

    void Application::OnMouseButton(int button, int action, int mods)
    {
        if (this->m_menu != nullptr && action == GLFW_PRESS)
        {
            int scale = CalculateGuiScale(this->window.GetWidth(), this->window.GetHeight());
            this->m_menu->MouseClicked(this->m_mousePos[0] / scale, this->m_mousePos[1] / scale, button);
        }

        if (button == 1 && action == GLFW_RELEASE) {
            if (m_mouseGrabbed && m_hitResult.has_value()) {
                int dX = m_hitResult.value().x;
                int dY = m_hitResult.value().y;
                int dZ = m_hitResult.value().z;

                int* directionVector = directionNormalVectors[m_hitResult.value().face];

                dX += directionVector[0];
                dY += directionVector[1];
                dZ += directionVector[2];

                if (this->m_player->bb.intersects({ (float) dX, (float) dY, (float) dZ, dX + 1.0f, dY + 0.5f, dZ + 1.0f }))
                    return;

                m_world->setBlockId(dX, dY, dZ, m_selectedItem + 1);
                m_world->recalculateLightDepths(dX, dZ, 1, 1);

                int chunksX = m_world->getWidth() / 16;
                int chunksZ = m_world->getHeight() / 16;

                int chunkX = dX / 16;
                int chunkZ = dZ / 16;

                int localBX = dX & 15;
                int localBY = dY;
                int localBZ = dZ & 15;

                m_chunks[chunkZ * chunksX + chunkX]->dirty = true;
                
                if (localBX == 0 && chunkX > 0) {
                    m_chunks[(chunkZ * chunksX) + (chunkX - 1)]->dirty = true;
                }

                if (localBX == 15 && chunkX < chunksX - 1) {
                    m_chunks[(chunkZ * chunksX) + (chunkX + 1)]->dirty = true;
                }

                if (localBZ == 0 && chunkZ > 0) {
                    m_chunks[((chunkZ - 1) * chunksX) + chunkX]->dirty = true;
                }

                if (localBZ == 15 && chunkZ < chunksZ - 1) {
                    m_chunks[((chunkZ + 1) * chunksX) + chunkX]->dirty = true;
                }
            }
        }

        if (button == 0 && action == GLFW_RELEASE) {
            if (!m_mouseGrabbed) {
                // glfwSetInputMode(window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                // m_mouseGrabbed = true;
            } else {
                if (m_hitResult.has_value()) {
                    int localBX = m_hitResult.value().x & 15;
                    int localBY = m_hitResult.value().y;
                    int localBZ = m_hitResult.value().z & 15;

                    int chunkX = m_hitResult.value().x / 16;
                    int chunkZ = m_hitResult.value().z / 16;

                    m_world->setBlockId(m_hitResult.value().x, m_hitResult.value().y, m_hitResult.value().z, 0);
                    m_world->recalculateLightDepths(m_hitResult.value().x, m_hitResult.value().z, 1, 1);

                    int chunksX = m_world->getWidth() / 16;
                    int chunksZ = m_world->getHeight() / 16;
                    m_chunks[(m_hitResult.value().z / 16) * chunksX + (m_hitResult.value().x / 16)]->dirty = true;
                
                    if (localBX == 0 && chunkX > 0) {
                        m_chunks[(chunkZ * chunksX) + (chunkX - 1)]->dirty = true;
                    }

                    if (localBX == 15 && chunkX < chunksX - 1) {
                        m_chunks[(chunkZ * chunksX) + (chunkX + 1)]->dirty = true;
                    }

                    if (localBZ == 0 && chunkZ > 0) {
                        m_chunks[((chunkZ - 1) * chunksX) + chunkX]->dirty = true;
                    }

                    if (localBZ == 15 && chunkZ < chunksZ - 1) {
                        m_chunks[((chunkZ + 1) * chunksX) + chunkX]->dirty = true;
                    }
                }
            }
        }
    }

    void Application::OnCursorPos(double x, double y)
    {
        if (this->m_mouseGrabbed) {
            this->m_mouseDelta[0] = x - this->m_mousePos[0];
            this->m_mouseDelta[1] = this->m_mousePos[1] - y;
        }
        this->m_mousePos[0] = x;
        this->m_mousePos[1] = y;
    }

    void Application::OnScroll(double x, double y)
    {
        if (y > 0) {
            this->m_selectedItem -= 1;
            if (this->m_selectedItem < 0) {
                this->m_selectedItem = 8;
            }
        }

        if (y < 0) {
            this->m_selectedItem += 1;
            if (this->m_selectedItem > 8) {
                this->m_selectedItem = 0;
            }
        }
    }

    Application* Application::GetInstance()
    {
        return appInstance;
    }

    void Application::SetMenu(Menu* menu)
    {
        if (this->m_menu != nullptr) {
            delete this->m_menu;
        }

        this->m_menu = menu;

        if (this->m_menu != nullptr) {
            int scale = CalculateGuiScale(this->window.GetWidth(), this->window.GetHeight());
            this->m_menu->Init(this->window.GetWidth() / scale, this->window.GetHeight() / scale);
        } else {
            glfwSetCursorPos(this->window.GetHandle(), this->window.GetWidth() / 2, this->window.GetHeight() / 2);
            glfwSetInputMode(this->window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            this->m_mouseGrabbed = true;
        }
    }

    void Application::Shutdown()
    {
        this->m_running = false;
    }

    void Application::StartWorld()
    {
        m_world = new World(128, 128, 64);

        int chunksX = m_world->getWidth() / 16;
        int chunksZ = m_world->getHeight() / 16;
        
        for (int z = 0; z < chunksZ; ++z) {
            for (int x = 0; x < chunksX; ++x) {
                KM::Chunk* chunk = new KM::Chunk(m_world, x * 16, 0, z * 16);
                m_chunks.push_back(chunk); 
            }
        }
        m_player = new Player();
        m_player->setWorld(m_world);
        this->SetMenu(nullptr);
    }

    void Application::QuitWorld()
    {
        std::cout << "Clearing Chunks\n";
        for (KM::Chunk* chunk : m_chunks) {
            delete chunk;
        }
        m_chunks.clear();

        delete m_player;
        m_player = nullptr;
        delete m_world;
        m_world = nullptr;

        this->SetMenu(new TitleMenu());
    }

    void Application::Render()
    {
        if (this->m_world != nullptr) {
            this->RenderWorld();
        
            glDisable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT);
        } else {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        this->RenderGui();
    }

    void Application::Tick()
    {
        if (this->m_menu == nullptr && this->m_player != nullptr) {
            this->m_player->tick();
        
            float mouseDeltaX = (float) this->m_mouseDelta[0];
            float mouseDeltaY = (float) this->m_mouseDelta[1];
            this->m_mouseDelta[0] = 0;
            this->m_mouseDelta[1] = 0;

            this->m_player->turn(mouseDeltaX, mouseDeltaY);
        
            glm::vec3 start = glm::vec3(m_player->x, m_player->y, m_player->z);

            float pitch = glm::radians(-m_player->pitch);
            float yaw = glm::radians(-m_player->yaw + 180.0f);        

            glm::vec3 dir;
            dir.x = glm::cos(pitch) * glm::sin(yaw);
            dir.y = glm::sin(pitch);
            dir.z = glm::cos(pitch) * glm::cos(yaw);
            dir = glm::normalize(dir);
            
            float reach = 5.0f;
            glm::vec3 end = start + dir * reach;
            m_hitResult = m_world->raycast(start, end);
        }
    }

    void Application::RenderWorld()
    {
        bool isInsideWater = this->m_player->isInsideWater;
     
        float fogR = isInsideWater ? 0.1f : 0.5f;
        float fogG = isInsideWater ? 0.3f : 0.8f;
        float fogB = isInsideWater ? 0.6f : 1.0f;

        glClearColor(fogR, fogG, fogB, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        int w = window.GetWidth();
        int h = window.GetHeight();

        glEnable(GL_CULL_FACE);

        glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float) w / (float) h, 0.05f, 1000.0f);
        glm::mat4 modelView = glm::mat4(1.0f);
        modelView = glm::translate(modelView, glm::vec3(0.0f, 0.0f, -0.3f));
        modelView = glm::rotate(modelView, glm::radians(m_player->pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        modelView = glm::rotate(modelView, glm::radians(m_player->yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        modelView = glm::translate(modelView, glm::vec3(-m_player->x, -m_player->y, -m_player->z));

        renderer.terrainShader.Use();
        renderer.terrainShader.SetUniformMat4("uProjection", projection);
        renderer.terrainShader.SetUniformMat4("uModelView", modelView);
        renderer.terrainShader.SetUniformFloat("uFogStart", 50 + (isInsideWater ? -45 : 0));
        renderer.terrainShader.SetUniformFloat("uFogEnd", 82 + (isInsideWater ? -45 : 0));
        renderer.terrainShader.SetUniformFloat4("uFogColor", fogR, fogG, fogB, 1.0f);
        renderer.terrainShader.SetUniformFloat4("uPlayer", (float) m_player->x, (float) m_player->z, 0.0f, 0.0f);

       glm::quat a(0.0f, 0.0f, 0.0f, 1.0f);
        {
            float yaw = m_player->yaw + 180.0f;
            float pitch = m_player->pitch;
            a = glm::rotate(a, glm::pi<float>() - yaw * ((float)glm::pi<float>() / 180), glm::vec3(0.0f, 1.0f, 0.0f));
            a = glm::rotate(a, -pitch * (glm::pi<float>() / 180), glm::vec3(1.0f, 0.0f, 0.0f));
            a = glm::rotate(a, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            a = glm::quat(-a.x, -a.y, -a.z, a.w);
        }
        auto m = glm::mat4_cast(a);

        Frustum frustum(projection * modelView);

        renderer.terrainTexture.Bind(0);
        renderer.terrainShader.SetUniformInt("uSampler", 0);
        
        for (auto chunk : m_chunks) {
            chunkTotal++;
            if (!chunk->IsInFrustum(frustum)) continue;
            chunk->render(0);
            chunkRendered++;
        }

        for (auto chunk : m_chunks) {
            chunk->render(1);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto chunk : m_chunks) {
            chunk->render(2);
        }
        
        if (m_hitResult.has_value()) {
            renderer.defaultShader.Use();
            renderer.defaultShader.SetUniformMat4("uProjection", projection);
            modelView = glm::translate(modelView, glm::vec3(m_hitResult.value().x, m_hitResult.value().y, m_hitResult.value().z));
            renderer.defaultShader.SetUniformMat4("uModelView", modelView);

            glBindVertexArray(m_selectionBoxVao);
            glDrawArrays(GL_LINES, 0, m_selectionVertexCount);
            glBindVertexArray(0);
        }

        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }

    void Application::RenderGui()
    {
        int w = window.GetWidth();
        int h = window.GetHeight();

        int scale = CalculateGuiScale(w, h);
        int scaledWidth = w / scale;
        int scaledHeight = h / scale;

        glm::mat4 projection = glm::ortho(0.0f, (float) scaledWidth, (float) scaledHeight, 0.0f, 1000.0f, 3000.0f);
        glm::mat4 modelView = glm::mat4(1.0f);  
        modelView = glm::translate(modelView, glm::vec3(0.0f, 0.0f, -2000.0f));

        renderer.texturedShader.Use();
        renderer.texturedShader.SetUniformMat4("uProjection", projection);
        renderer.texturedShader.SetUniformMat4("uModelView", modelView);

        if (this->m_world != nullptr && this->m_player != nullptr)
        {
            std::string version = (const char*) glGetString(GL_VERSION);
            version = version.substr(0, version.find_first_of(' '));
            std::string rrenderer = (const char*) glGetString(GL_RENDERER);
            std::string vendor = (const char*) glGetString(GL_VENDOR);

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
            this->renderer.DrawTexture(this->renderer.m_guiTexture, scaledWidth / 2 - 8, scaledHeight / 2 - 8, 16, 16, 256 - 16, 0, 16, 16, 256, 256);
            glDisable(GL_BLEND);

            if (m_showDebugInfo) {
                renderer.DrawTextWithShadow("NoCraft (" + std::to_string(KM::Chunk::chunkUpdates) + " chunk updates)", 2, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                renderer.DrawTextWithShadow(m_fpsString + " CR " + std::to_string(chunkRendered) + "/" + std::to_string(chunkTotal), 2, 12, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                renderer.DrawTextWithShadow("X: " + std::to_string(m_player->x), 2, 32, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                renderer.DrawTextWithShadow("Y: " + std::to_string(m_player->y), 2, 42, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                renderer.DrawTextWithShadow("Z: " + std::to_string(m_player->z), 2, 52, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                renderer.DrawTextWithShadow("Pitch: " + std::to_string(m_player->pitch), 2, 62, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                renderer.DrawTextWithShadow("Yaw: " + std::to_string(m_player->yaw), 2, 72, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

                if (m_hitResult.has_value()) {
                    renderer.DrawTextWithShadow("H XYZ: " + std::to_string(m_hitResult.value().x) + " " + std::to_string(m_hitResult.value().y) + " " + std::to_string(m_hitResult.value().z), 2, 92, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                    renderer.DrawTextWithShadow("H Face: " + std::to_string(m_hitResult.value().face), 2, 102, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                }

                std::string line = "Display: " + std::to_string(w) + "x" + std::to_string(h) + " (" + vendor + ")";
                renderer.DrawAlignedTextWithShadow(line, scaledWidth - 2, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
                renderer.DrawAlignedTextWithShadow(rrenderer, scaledWidth - 2, 12, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
                renderer.DrawAlignedTextWithShadow(version, scaledWidth - 2, 22, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
            } else {
                renderer.DrawTextWithShadow("NoCraft", 2, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            }

            glEnable(GL_BLEND);
            renderer.DefaultBlendFunc();
            this->renderer.DrawTexture(this->renderer.m_guiTexture, scaledWidth / 2 - 91, scaledHeight - 22, 182, 22, 0, 0, 182, 22, 256, 256);
            this->renderer.DrawTexture(this->renderer.m_guiTexture, scaledWidth / 2 - 91 + m_selectedItem * 20 + 1 - 2, scaledHeight - 22 - 1, 24, 24, 0, 22, 24, 24, 256, 256);
            glDisable(GL_BLEND);

            for (int i = 0; i < BLOCK_COUNT; ++i) {
                BlockDef* block = &BLOCKS_DEFS[i + 1];
                {
                    glEnable(GL_DEPTH_TEST);
                    glm::mat4 mv = modelView;
                    mv = glm::translate(mv, glm::vec3(scaledWidth / 2 - 91 + 1 + i * 20 - 1 + 3 - 29, scaledHeight - 22 + 2 + 8.5f, 10));
                    mv = glm::scale(mv, glm::vec3(10, 10, 10));
                    mv = glm::translate(mv, glm::vec3(1.0f, 0.5f, 1.0f));
                    mv = glm::scale(mv, glm::vec3(1.0f, 1.0f, -1.0f));
                    mv = glm::rotate(mv, glm::radians(210.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    mv = glm::rotate(mv, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    renderer.texturedShader.SetUniformMat4("uModelView", mv);

                    renderer.terrainTexture.Bind(0);
                    renderer.terrainShader.SetUniformInt("uSampler", 0);
                    std::vector<Vertex> verts;
                    glEnable(GL_BLEND);
                    this->renderer.DefaultBlendFunc();
                    glBindVertexArray(this->renderer.m_guiVao);
                    glBindBuffer(GL_ARRAY_BUFFER, this->renderer.m_guiVbo);
                    this->RenderTileGui(verts, i);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), &verts[0], GL_DYNAMIC_DRAW);
                    glDrawArrays(GL_TRIANGLES, 0, verts.size());
                    glDisable(GL_BLEND);
                    glBindVertexArray(0);
                    glDisable(GL_DEPTH_TEST);
                }
                // this->renderer.DrawTexture(this->renderer.terrainTexture, scaledWidth / 2 - 91 + 1 + i * 20 - 1 + 3, scaledHeight - 22 + 3, 16, 16, (block->guiTextureIndex % 16) * 16, (block->guiTextureIndex / 16) * 16, 16, 16, 256, 256);
            }
        }

        if (this->m_menu != nullptr) {
            renderer.texturedShader.SetUniformMat4("uModelView", modelView);

            this->m_menu->Render(this->m_mousePos[0] / scale, this->m_mousePos[1] / scale);
        }
    }

    void Application::RenderSelectionBox(std::vector<KM::Vertex3FColor4F> &vertices, KM::HitResult &hitResult) {
        float x0 = -0.005f;
        float y0 = -0.005f;
        float z0 = -0.005f;
        float x1 = 1.005f;
        float y1 = 1.005f;
        float z1 = 1.005f;

        vertices.push_back({ x0, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x0, y1, z0, 0, 0, 0, 0.4f });

        vertices.push_back({ x1, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y1, z0, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y0, z1, 0, 0, 0, 0.4f });
        vertices.push_back({ x0, y1, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x1, y0, z1, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y1, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y1, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x0, y1, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x0, y0, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y0, z0, 0, 0, 0, 0.4f });

        vertices.push_back({ x1, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y0, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x1, y1, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y1, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y1, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y1, z0, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y1, z1, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y1, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y0, z1, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y0, z1, 0, 0, 0, 0.4f });
    }
    
    void Application::RenderTile(std::vector<KM::Vertex> &vertices, KM::World* world, BlockPos blockPos, int tile, int layer)
    {
        if (tile == 0 || tile >= BLOCK_COUNT) return;

        BlockDef* block = &BLOCKS_DEFS[tile];

        if (layer != block->renderLayer) {
            return;
        }

        float x0 = blockPos.x;
        float y0 = blockPos.y;
        float z0 = blockPos.z;
        float x1 = blockPos.x + 1.0f;
        float y1 = blockPos.y + 1.0f;
        float z1 = blockPos.z + 1.0f;


        float bB = 0.5f;
        float bT = 1.0f;
        float bNS = 0.8f;
        float bWE = 0.6f;

        int blockIdDown = world->getBlockId(blockPos.Down());
        int blockIdUp = world->getBlockId(blockPos.Up());
        int blockIdNorth = world->getBlockId(blockPos.North());
        int blockIdSouth = world->getBlockId(blockPos.South());
        int blockIdWest = world->getBlockId(blockPos.West());
        int blockIdEast = world->getBlockId(blockPos.East());
        
        bool isExposed = blockIdUp == 0 && tile == 5;

        if (isExposed) {
            y1 -= 0.1f;
        }

        /* TODO: Finish AO
        */

        // Credits: https://github.com/simondevyoutube/Quick_MinecraftClone2/blob/f911091c593e78a06ec59b3f49656d6a3b8f4a79/src/voxel-block-builder.js#L955
        auto calcAO = [world, blockPos](int x, int y, int z) {
            int bId = world->getBlockId(blockPos.x + x, blockPos.y + y, blockPos.z + z);
            if (bId != 0 && bId != 8) {
                return 0.75f;
            }
          return 1.0f;
        };

        // B

        if (blockIdDown == 0 || blockIdDown == 7 || (blockIdDown == 8 && tile != 8) || (blockIdDown == 5 && tile != 5)) {
            int u = ((block->textureIndex[0] % 16) * 16);
            int v = ((block->textureIndex[0] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bB * world->getBlockBrightness(blockPos.Down());

            vertices.push_back({ x0, y0, z1, b, b, b, 1, u0, v1 });
            vertices.push_back({ x0, y0, z0, b, b, b, 1, u0, v0 });
            vertices.push_back({ x1, y0, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x1, y0, z1, b, b, b, 1, u1, v1 });
        }
        
        // T

        if (blockIdUp == 0 || blockIdUp == 7 || (blockIdUp == 8 && tile != 8) || (blockIdUp == 5 && tile != 5)) {
            int u = ((block->textureIndex[1] % 16) * 16);
            int v = ((block->textureIndex[1] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bT * world->getBlockBrightness(blockPos.Up());

            float aos[4] = {
                calcAO(0, 1, 1) * calcAO(1, 1, 0) * calcAO(1, 1, 1),
                calcAO(0, 1, -1) * calcAO(1, 1, 0) * calcAO(1, 1, -1),
                calcAO(0, 1, -1) * calcAO(-1, 1, 0) * calcAO(-1, 1, -1),
                calcAO(0, 1, 1) * calcAO(-1, 1, 0) * calcAO(-1, 1, 1),
            };

            float v0b = b * aos[0];
            float v1b = b * aos[1];
            float v2b = b * aos[2];
            float v3b = b * aos[3];

            vertices.push_back({ x1, y1, z1, v0b, v0b, v0b, 1, u1, v1 });
            vertices.push_back({ x1, y1, z0, v1b, v1b, v1b, 1, u1, v0 });
            vertices.push_back({ x0, y1, z0, v2b, v2b, v2b, 1, u0, v0 });
            vertices.push_back({ x0, y1, z1, v3b, v3b, v3b, 1, u0, v1 });

            if (tile == 5) {
                int u = ((block->textureIndex[0] % 16) * 16);
                int v = ((block->textureIndex[0] / 16) * 16);
                float u0 = u / 256.0f;
                float v0 = v / 256.0f;
                float u1 = (u + 16.f) / 256.0f;
                float v1 = (v + 16.f) / 256.0f;

                float b = bB * world->getBlockBrightness(blockPos.Down());

                vertices.push_back({ x0, y1, z1, b, b, b, 1, u0, v1 });
                vertices.push_back({ x0, y1, z0, b, b, b, 1, u0, v0 });
                vertices.push_back({ x1, y1, z0, b, b, b, 1, u1, v0 });
                vertices.push_back({ x1, y1, z1, b, b, b, 1, u1, v1 });
            }
        }

        // N

        if (blockIdNorth == 0 || blockIdNorth == 7 || (blockIdNorth == 8 && tile != 8)|| (blockIdNorth == 5 && tile != 5)) {
            int u = ((block->textureIndex[2] % 16) * 16);
            int v = ((block->textureIndex[2] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bNS * world->getBlockBrightness(blockPos.North());

            float aos[4] = {
                calcAO(-1, 0, -1) * calcAO(0, 1, -1) * calcAO(-1, 1, -1),
                calcAO(1, 0, -1) * calcAO(0, 1, -1) * calcAO(1, 1, -1),
                calcAO(1, 0, -1) * calcAO(0, -1, -1) * calcAO(1, -1, -1),
                calcAO(-1, 0, -1) * calcAO(0, -1, -1) * calcAO(-1, -1, -1),
            };

            float v0b = b * aos[0];
            float v1b = b * aos[1];
            float v2b = b * aos[2];
            float v3b = b * aos[3];

            vertices.push_back({ x0, y1, z0, v0b, v0b, v0b, 1, u1, v0 });
            vertices.push_back({ x1, y1, z0, v1b, v1b, v1b, 1, u0, v0 });
            vertices.push_back({ x1, y0, z0, v2b, v2b, v2b, 1, u0, v1 });
            vertices.push_back({ x0, y0, z0, v3b, v3b, v3b, 1, u1, v1 });
        }

        // S

        if (blockIdSouth == 0 || blockIdSouth == 7 || (blockIdSouth == 8 && tile != 8) || (blockIdSouth == 5 && tile != 5)) {
            int u = ((block->textureIndex[3] % 16) * 16);
            int v = ((block->textureIndex[3] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bNS * world->getBlockBrightness(blockPos.South());

            float aos[4] = {
                calcAO(-1, 0, 1) * calcAO(0, 1, 1) * calcAO(-1, 1, 1),
                calcAO(1, 0, 1) * calcAO(0, -1, 1) * calcAO(1, -1, 1),
                calcAO(-1, 0, 1) * calcAO(0, -1, 1) * calcAO(-1, -1, 1),
                calcAO(1, 0, 1) * calcAO(0, 1, 1) * calcAO(1, 1, 1),
            };

            float v0b = b * aos[0];
            float v1b = b * aos[1];
            float v2b = b * aos[2];
            float v3b = b * aos[3];

            vertices.push_back({ x0, y1, z1, v0b, v0b, v0b, 1, u0, v0 });
            vertices.push_back({ x0, y0, z1, v1b, v1b, v1b, 1, u0, v1 });
            vertices.push_back({ x1, y0, z1, v2b, v2b, v2b, 1, u1, v1 });
            vertices.push_back({ x1, y1, z1, v3b, v3b, v3b, 1, u1, v0 });
        }

        // W

        if (blockIdWest == 0 || blockIdWest == 7 || (blockIdWest == 8 && tile != 8) || (blockIdWest == 5 && tile != 5)) {
            int u = ((block->textureIndex[4] % 16) * 16);
            int v = ((block->textureIndex[4] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bWE * world->getBlockBrightness(blockPos.West());

            float aos[4] = {
                calcAO(-1, 0, -1) * calcAO(-1, 1, 0) * calcAO(-1, 1, -1),
                calcAO(-1, 0, 1) * calcAO(-1, 1, 0) * calcAO(-1, 1, 1),
                calcAO(-1, 0, -1) * calcAO(-1, -1, 0) * calcAO(-1, -1, -1),
                calcAO(-1, 0, 1) * calcAO(-1, -1, 0) * calcAO(-1, -1, 1),
            };

            float v0b = b * aos[0];
            float v1b = b * aos[1];
            float v2b = b * aos[2];
            float v3b = b * aos[3];

            vertices.push_back({ x0, y1, z1, v0b, v0b, v0b, 1, u1, v0 });
            vertices.push_back({ x0, y1, z0, v1b, v1b, v1b, 1, u0, v0 });
            vertices.push_back({ x0, y0, z0, v2b, v2b, v2b, 1, u0, v1 });
            vertices.push_back({ x0, y0, z1, v3b, v3b, v3b, 1, u1, v1 });
        }

        // E

        if (blockIdEast == 0 || blockIdEast == 7 || (blockIdEast == 8 && tile != 8) || (blockIdEast == 5 && tile != 5)) {
            int u = ((block->textureIndex[5] % 16) * 16);
            int v = ((block->textureIndex[5] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bWE * world->getBlockBrightness(blockPos.East());

            float aos[4] = {
                calcAO(1, 0, 1) * calcAO(1, -1, 0) * calcAO(1, -1, 1),
                calcAO(1, 0, -1) * calcAO(1, -1, 0) * calcAO(1, -1, -1),
                calcAO(1, 0, 1) * calcAO(1, 1, 0) * calcAO(1, 1, 1),
                calcAO(1, 0, -1) * calcAO(1, 1, 0) * calcAO(1, 1, -1),
            };

            float v0b = b * aos[0];
            float v1b = b * aos[1];
            float v2b = b * aos[2];
            float v3b = b * aos[3];

            vertices.push_back({ x1, y0, z1, v0b, v0b, v0b, 1, u0, v1 });
            vertices.push_back({ x1, y0, z0, v1b, v1b, v1b, 1, u1, v1 });
            vertices.push_back({ x1, y1, z0, v2b, v2b, v2b, 1, u1, v0 });
            vertices.push_back({ x1, y1, z1, v3b, v3b, v3b, 1, u0, v0 });
        }
    }

    void Application::RenderTileGui(std::vector<KM::Vertex> &vertices, int tile)
    {
        if (tile == 0 || tile >= BLOCK_COUNT) return;

        BlockDef* block = &BLOCKS_DEFS[tile];


        float x0 = 0.0f;
        float y0 = 0.0f;
        float z0 = 0.0f;
        float x1 = 0.0f + 1.0f;
        float y1 = 0.0f + 1.0f;
        float z1 = 0.0f + 1.0f;


        float bB = 0.5f;
        float bT = 1.0f;
        float bNS = 0.8f;
        float bWE = 0.6f;

        bool isExposed = tile == 5;

        if (isExposed) {
            y1 -= 0.1f;
        }

        // B

        {
            int u = ((block->textureIndex[0] % 16) * 16);
            int v = ((block->textureIndex[0] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bB;

            vertices.push_back({ x0, y0, z1, b, b, b, 1, u0, v1 });
            vertices.push_back({ x0, y0, z0, b, b, b, 1, u0, v0 });
            vertices.push_back({ x1, y0, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x1, y0, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x1, y0, z1, b, b, b, 1, u1, v1 });
            vertices.push_back({ x0, y0, z1, b, b, b, 1, u0, v1 });
        }
        
        // T

        {
            int u = ((block->textureIndex[1] % 16) * 16);
            int v = ((block->textureIndex[1] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bT;

            vertices.push_back({ x1, y1, z1, b, b, b, 1, u1, v1 });
            vertices.push_back({ x1, y1, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x0, y1, z0, b, b, b, 1, u0, v0 });
            vertices.push_back({ x0, y1, z0, b, b, b, 1, u0, v0 });
            vertices.push_back({ x0, y1, z1, b, b, b, 1, u0, v1 });
            vertices.push_back({ x1, y1, z1, b, b, b, 1, u1, v1 });
        }

        // N

        {
            int u = ((block->textureIndex[2] % 16) * 16);
            int v = ((block->textureIndex[2] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bNS;

            vertices.push_back({ x0, y1, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x1, y1, z0, b, b, b, 1, u0, v0 });
            vertices.push_back({ x1, y0, z0, b, b, b, 1, u0, v1 });
            vertices.push_back({ x1, y0, z0, b, b, b, 1, u0, v1 });
            vertices.push_back({ x0, y0, z0, b, b, b, 1, u1, v1 });
            vertices.push_back({ x0, y1, z0, b, b, b, 1, u1, v0 });
        }

        // S

        {
            int u = ((block->textureIndex[3] % 16) * 16);
            int v = ((block->textureIndex[3] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bNS;

            vertices.push_back({ x0, y1, z1, b, b, b, 1, u0, v0 });
            vertices.push_back({ x0, y0, z1, b, b, b, 1, u0, v1 });
            vertices.push_back({ x1, y0, z1, b, b, b, 1, u1, v1 });
            vertices.push_back({ x1, y0, z1, b, b, b, 1, u1, v1 });
            vertices.push_back({ x1, y1, z1, b, b, b, 1, u1, v0 });
            vertices.push_back({ x0, y1, z1, b, b, b, 1, u0, v0 });
        }

        // W

        {
            int u = ((block->textureIndex[4] % 16) * 16);
            int v = ((block->textureIndex[4] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bWE;

            vertices.push_back({ x0, y1, z1, b, b, b, 1, u1, v0 });
            vertices.push_back({ x0, y1, z0, b, b, b, 1, u0, v0 });
            vertices.push_back({ x0, y0, z0, b, b, b, 1, u0, v1 });
            vertices.push_back({ x0, y0, z0, b, b, b, 1, u0, v1 });
            vertices.push_back({ x0, y0, z1, b, b, b, 1, u1, v1 });
            vertices.push_back({ x0, y1, z1, b, b, b, 1, u1, v0 });
        }

        // E

        {
            int u = ((block->textureIndex[5] % 16) * 16);
            int v = ((block->textureIndex[5] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bWE;

            vertices.push_back({ x1, y0, z1, b, b, b, 1, u0, v1 });
            vertices.push_back({ x1, y0, z0, b, b, b, 1, u1, v1 });
            vertices.push_back({ x1, y1, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x1, y1, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x1, y1, z1, b, b, b, 1, u0, v0 });
            vertices.push_back({ x1, y0, z1, b, b, b, 1, u0, v1 });
        }
    }

    int Application::CalculateGuiScale(int width, int height) {
        int scale;
        for (scale = 1; scale != 3 && scale < width && scale < height && width / (scale + 1) >= 320 && height / (scale + 1) >= 240; ++scale) {
        }
        return scale;
    }
}