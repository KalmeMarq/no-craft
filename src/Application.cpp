#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Application.hpp"

struct BlockDef {
    int guiTextureIndex;
    int textureIndex[6];
};

#define BLOCK_COUNT 10
static BlockDef BLOCKS_DEFS[BLOCK_COUNT] = {
    { 0, { 0 } }, // Air
    { 3, { 2, 0, 3, 3, 3, 3 } }, // Grass
    { 2, { 2, 2, 2, 2, 2, 2 } }, // Dirt
    { 1, { 1, 1, 1, 1, 1, 1 } }, // Stone
    { 4, { 4, 4, 4, 4, 4, 4 } }, // Planks
    { 16, { 16, 16, 16, 16, 16, 16 } }, // Cobblestone
    { 17, { 17, 17, 17, 17, 17, 17 } }, // Bedrock
    { 22, { 22, 22, 22, 22, 22, 22 } }, // Leaves
    { 49, { 49, 49, 49, 49, 49, 49 } }, // Glass
    { 20, { 21, 21, 20, 20, 20, 20 } } // Log
};

namespace KM {
    Application *Application::appInstance { nullptr };

    Application::Application() : m_textRenderer(&m_texturedShader), m_world(128, 128, 64), m_menu(nullptr)
    {
        appInstance = this;
        m_player.setWorld(&m_world);
    }

    Application::~Application()
    {
        if (this->m_menu != nullptr) {
            delete this->m_menu;
        }

        for (KM::Chunk *chunk : m_chunks) {
            delete chunk;
        }
        m_chunks.clear();
        std::cout << "Closing...\n";
    }

    void Application::Run()
    {
        std::cout << "Creating Window\n";
        m_window.Init(854, 480, "NoCraft");
        m_window.SetVsync(false);

        std::cout << "Initializing Shaders\n";
        m_defaultShader.Init("default");
        m_texturedShader.Init("textured");
        m_terrainShader.Init("terrain");
    
        std::cout << "Loading Textures\n";
        m_terrainTexture.LoadFromFile("terrain.png");
        m_guiTexture.LoadFromFile("gui.png");

        std::cout << "Initializing TextRenderer\n";
        m_textRenderer.Init();

        std::cout << "Creating Chunks\n";
        int chunksX = m_world.getWidth() / 16;
        int chunksZ = m_world.getHeight() / 16;

        for (int z = 0; z < chunksZ; ++z) {
            for (int x = 0; x < chunksX; ++x) {
                KM::Chunk *chunk = new KM::Chunk(&m_world, x * 16, 0, z * 16);
                m_chunks.push_back(chunk); 
            }
        }

        std::cout << "GUI\n";

        glGenVertexArrays(1, &m_guiVao);
        glBindVertexArray(m_guiVao);
        glGenBuffers(1, &m_guiVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_guiVbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid *)((3 + 4) * sizeof(float)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

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

        m_window.AddResizeHandler([this]() { this->OnResize(); });
        m_window.AddKeyboardHandler([this](int key, int scancode, int action, int mods) { this->OnKey(key, scancode, action, mods); });
        m_window.AddMouseButtonHandler([this](int button, int action, int mods) { this->OnMouseButton(button, action, mods); });
        m_window.AddScrollHandler([this](double x, double y) { this->OnScroll(x, y); });
        m_window.AddCursorPosHandler([this](double x, double y) { this->OnCursorPos(x, y); });

        double lastTime = glfwGetTime();
        int frameCounter = 0;
        int tickCounter = 0;

        double tickLastTime = glfwGetTime();
        double tickTimeBehind = 0.0;
        double tickTargetTimeStep = 1.0 / 60.0;

        this->SetMenu(new GameMenu());

        while (this->m_running)
        {
            if (m_window.ShouldClose()) Shutdown();

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

            m_window.Update();
            frameCounter++;

            while (glfwGetTime() - lastTime > 1.0) {
                lastTime += 1.0;
                m_fpsString = std::to_string(frameCounter) + " FPS " + std::to_string(tickCounter) + " TPS";
                KM::Chunk::chunkUpdates = 0;
                frameCounter = 0;
                tickCounter = 0;
            }
        }
    }

    void Application::OnResize()
    {
        std::cout << "Viewport Resized";
        glViewport(0, 0, this->m_window.GetWidth(), this->m_window.GetHeight());

        if (this->m_menu != nullptr)
        {
            int scale = CalculateGuiScale(this->m_window.GetWidth(), this->m_window.GetHeight());
            this->m_menu->Resize(this->m_window.GetWidth() / scale, this->m_window.GetHeight() / scale);
        }
    }

    void Application::OnKey(int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
            if (this->m_mouseGrabbed) {
                glfwSetInputMode(this->m_window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetCursorPos(this->m_window.GetHandle(), this->m_window.GetWidth() / 2, this->m_window.GetHeight() / 2);
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
            int scale = CalculateGuiScale(this->m_window.GetWidth(), this->m_window.GetHeight());
            this->m_menu->MouseClicked(this->m_mousePos[0] / scale, this->m_mousePos[1] / scale, button);
        }

        if (button == 1 && action == GLFW_RELEASE) {
            if (m_mouseGrabbed && m_hitResult.has_value()) {
                int dX = m_hitResult.value().x;
                int dY = m_hitResult.value().y;
                int dZ = m_hitResult.value().z;

                if (m_hitResult.value().face == 0) {
                    dY -= 1;
                }

                if (m_hitResult.value().face == 1) {
                    dY += 1;
                }

                if (m_hitResult.value().face == 2) {
                    dZ -= 1;
                }

                if (m_hitResult.value().face == 3) {
                    dZ += 1;
                }

                if (m_hitResult.value().face == 4) {
                    dX -= 1;
                }

                if (m_hitResult.value().face == 5) {
                    dX += 1;
                }

                m_world.setBlockId(dX, dY, dZ, m_selectedItem + 1);
                m_world.recalculateLightDepths(dX, dZ, 1, 1);

                int chunksX = m_world.getWidth() / 16;
                int chunksZ = m_world.getHeight() / 16;

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
                // glfwSetInputMode(m_window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                // m_mouseGrabbed = true;
            } else {
                if (m_hitResult.has_value()) {
                    int localBX = m_hitResult.value().x & 15;
                    int localBY = m_hitResult.value().y;
                    int localBZ = m_hitResult.value().z & 15;

                    int chunkX = m_hitResult.value().x / 16;
                    int chunkZ = m_hitResult.value().z / 16;

                    m_world.setBlockId(m_hitResult.value().x, m_hitResult.value().y, m_hitResult.value().z, 0);
                    m_world.recalculateLightDepths(m_hitResult.value().x, m_hitResult.value().z, 1, 1);

                    int chunksX = m_world.getWidth() / 16;
                    int chunksZ = m_world.getHeight() / 16;
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

    Application *Application::GetInstance()
    {
        return appInstance;
    }

    void Application::SetMenu(Menu *menu)
    {
        if (this->m_menu != nullptr) {
            delete this->m_menu;
        }

        this->m_menu = menu;

        if (this->m_menu != nullptr) {
            int scale = CalculateGuiScale(this->m_window.GetWidth(), this->m_window.GetHeight());
            this->m_menu->Init(this->m_window.GetWidth() / scale, this->m_window.GetHeight() / scale);
        } else {
            glfwSetCursorPos(this->m_window.GetHandle(), this->m_window.GetWidth() / 2, this->m_window.GetHeight() / 2);
            glfwSetInputMode(this->m_window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            this->m_mouseGrabbed = true;
        }
    }

    void Application::Shutdown()
    {
        this->m_running = false;
    }

    void Application::Render()
    {
        glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        this->RenderWorld();

        glDisable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        this->RenderGui();
    }

    void Application::Tick()
    {
        this->m_player.xo = this->m_player.x;
        this->m_player.yo = this->m_player.y;
        this->m_player.zo = this->m_player.z;

        float xa = 0;
        float za = 0;
        
        if (glfwGetKey(this->m_window.GetHandle(), GLFW_KEY_SPACE) != GLFW_RELEASE && this->m_player.onGround) {
            this->m_player.yd = 0.12f;
        }

        if (glfwGetKey(this->m_window.GetHandle(), GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE || glfwGetKey(this->m_window.GetHandle(), GLFW_KEY_RIGHT_SHIFT) != GLFW_RELEASE && !this->m_player.onGround) {
            this->m_player.yd = -0.12f;
        }

        if (glfwGetKey(this->m_window.GetHandle(), GLFW_KEY_W) != GLFW_RELEASE) {
            za -= 1;
        }

        if (glfwGetKey(this->m_window.GetHandle(), GLFW_KEY_S) != GLFW_RELEASE) {
            za += 1;
        }

        if (glfwGetKey(this->m_window.GetHandle(), GLFW_KEY_A) != GLFW_RELEASE) {
            xa -= 1;
        }

        if (glfwGetKey(this->m_window.GetHandle(), GLFW_KEY_D) != GLFW_RELEASE) {
            xa += 1;
        }

        this->m_player.mouseRelative(xa, za, this->m_player.onGround ? 0.02f : 0.005f);
        this->m_player.yd = (float) ((double) this->m_player.yd - 0.005);
        this->m_player.move(this->m_player.xd, this->m_player.yd, this->m_player.zd);
        this->m_player.xd *= 0.91f;
        this->m_player.yd *= 0.98f;
        this->m_player.zd *= 0.91f;

        if (this->m_player.onGround) {
            this->m_player.xd *= 0.8F;
            this->m_player.zd *= 0.8F;
        }

        float mouseDeltaX = (float) this->m_mouseDelta[0];
        float mouseDeltaY = (float) this->m_mouseDelta[1];
        this->m_mouseDelta[0] = 0;
        this->m_mouseDelta[1] = 0;

        this->m_player.turn(mouseDeltaX, mouseDeltaY);

        glm::vec3 start = glm::vec3(m_player.x, m_player.y, m_player.z);

        float pitch = glm::radians(-m_player.pitch);
        float yaw = glm::radians(-m_player.yaw + 180.0f);        

        glm::vec3 dir;
        dir.x = glm::cos(pitch) * glm::sin(yaw);
        dir.y = glm::sin(pitch);
        dir.z = glm::cos(pitch) * glm::cos(yaw);
        dir = glm::normalize(dir);
        
        float reach = 5.0f;
        glm::vec3 end = start + dir * reach;
        m_hitResult = m_world.raycast(start, end);
    }

    void Application::RenderWorld()
    {
        int w = m_window.GetWidth();
        int h = m_window.GetHeight();
        glViewport(0, 0, w, h);

        glEnable(GL_CULL_FACE);

        glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float) w / (float) h, 0.05f, 1000.0f);
        glm::mat4 modelView = glm::mat4(1.0f);
        modelView = glm::translate(modelView, glm::vec3(0.0f, 0.0f, -0.3f));
        modelView = glm::rotate(modelView, glm::radians(m_player.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        modelView = glm::rotate(modelView, glm::radians(m_player.yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        modelView = glm::translate(modelView, glm::vec3(-m_player.x, -m_player.y, -m_player.z));

        m_terrainShader.Use();
        m_terrainShader.SetUniformMat4("uProjection", projection);
        m_terrainShader.SetUniformMat4("uModelView", modelView);
        m_terrainShader.SetUniformFloat("uFogStart", 100);
        m_terrainShader.SetUniformFloat("uFogEnd", 132);
        m_terrainShader.SetUniformFloat4("uFogColor", 0.5f, 0.8f, 1.0f, 1.0f);
        m_terrainShader.SetUniformFloat4("uPlayer", (float) m_player.x, (float) m_player.z, 0.0f, 0.0f);

        m_terrainTexture.Bind(0);
        m_terrainShader.SetUniformInt("uSampler", 0);
        
        for (auto chunk : m_chunks) {
            chunk->render(0);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto chunk : m_chunks) {
            chunk->render(1);
        }
        
        if (m_hitResult.has_value()) {
            m_defaultShader.Use();
            m_defaultShader.SetUniformMat4("uProjection", projection);
            modelView = glm::translate(modelView, glm::vec3(m_hitResult.value().x, m_hitResult.value().y, m_hitResult.value().z));
            m_defaultShader.SetUniformMat4("uModelView", modelView);

            glBindVertexArray(m_selectionBoxVao);
            glDrawArrays(GL_LINES, 0, m_selectionVertexCount);
            glBindVertexArray(0);
        }

        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }

    void Application::RenderGui()
    {
        std::string version = (const char*) glGetString(GL_VERSION);
        version = version.substr(0, version.find_first_of(' '));
        std::string renderer = (const char*) glGetString(GL_RENDERER);
        std::string vendor = (const char*) glGetString(GL_VENDOR);

        int w = m_window.GetWidth();
        int h = m_window.GetHeight();

        int scale = CalculateGuiScale(w, h);
        int scaledWidth = w / scale;
        int scaledHeight = h / scale;

        glm::mat4 projection = glm::ortho(0.0f, (float) scaledWidth, (float) scaledHeight, 0.0f, 1000.0f, 3000.0f);
        glm::mat4 modelView = glm::mat4(1.0f);  
        modelView = glm::translate(modelView, glm::vec3(0.0f, 0.0f, -2000.0f));

        m_texturedShader.Use();
        m_texturedShader.SetUniformMat4("uProjection", projection);
        m_texturedShader.SetUniformMat4("uModelView", modelView);

        m_textRenderer.DrawText("+", scaledWidth / 2 - 4, scaledHeight / 2 - 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), false);

        if (m_showDebugInfo) {
            m_textRenderer.DrawText("NoCraft (" + std::to_string(KM::Chunk::chunkUpdates) + " chunk updates)", 2, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            m_textRenderer.DrawText(m_fpsString, 2, 12, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            m_textRenderer.DrawText("X: " + std::to_string(m_player.x), 2, 32, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            m_textRenderer.DrawText("Y: " + std::to_string(m_player.y), 2, 42, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            m_textRenderer.DrawText("Z: " + std::to_string(m_player.z), 2, 52, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            m_textRenderer.DrawText("Pitch: " + std::to_string(m_player.pitch), 2, 62, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            m_textRenderer.DrawText("Yaw: " + std::to_string(m_player.yaw), 2, 72, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            if (m_hitResult.has_value()) {
                m_textRenderer.DrawText("H XYZ: " + std::to_string(m_hitResult.value().x) + " " + std::to_string(m_hitResult.value().y) + " " + std::to_string(m_hitResult.value().z), 2, 92, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                m_textRenderer.DrawText("H Face: " + std::to_string(m_hitResult.value().face), 2, 102, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            }

            std::string line = "Display: " + std::to_string(w) + "x" + std::to_string(h) + " (" + vendor + ")";
            m_textRenderer.DrawAlignedText(line, scaledWidth - 2, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
            m_textRenderer.DrawAlignedText(renderer, scaledWidth - 2, 12, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
            m_textRenderer.DrawAlignedText(version, scaledWidth - 2, 22, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
        } else {
            m_textRenderer.DrawText("NoCraft", 2, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        m_guiTexture.Bind(0);
        m_texturedShader.SetUniformInt("uSampler", 0);
        DrawTexture(scaledWidth / 2 - 91, scaledHeight - 22, 182, 22, 0, 0, 182, 22, 256, 256);
        DrawTexture(scaledWidth / 2 - 91 + m_selectedItem * 20 + 1 - 2, scaledHeight - 22 - 1, 24, 24, 0, 22, 24, 24, 256, 256);

        m_terrainTexture.Bind(0);
        m_texturedShader.SetUniformInt("uSampler", 0);

        for (int i = 0; i < BLOCK_COUNT - 1; ++i) {
            BlockDef *block = &BLOCKS_DEFS[i + 1];
            DrawTexture(scaledWidth / 2 - 91 + 1 + i * 20 - 1 + 3, scaledHeight - 22 + 3, 16, 16, (block->guiTextureIndex % 16) * 16, (block->guiTextureIndex / 16) * 16, 16, 16, 256, 256);
        }

        if (this->m_menu != nullptr) {
            this->m_menu->Render(this->m_mousePos[0] / scale, this->m_mousePos[1] / scale);
        }
    }

    void Application::RenderSelectionBox(std::vector<KM::Vertex3FColor4F> &vertices, KM::HitResult &hitResult) {
        float x0 = -0.01f;
        float y0 = -0.01f;
        float z0 = -0.01f;
        float x1 = 1.01f;
        float y1 = 1.01f;
        float z1 = 1.01f;

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

        vertices.push_back({ x0, y1, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y1, z0, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y1, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x0, y1, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x0, y0, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y0, z0, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x0, y0, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y0, z0, 0, 0, 0, 0.4f });

        vertices.push_back({ x1, y0, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y0, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y1, z0, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y1, z0, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y1, z1, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y1, z1, 0, 0, 0, 0.4f });

        vertices.push_back({ x0, y0, z1, 0, 0, 0, 0.4f });
        vertices.push_back({ x1, y0, z1, 0, 0, 0, 0.4f });
    }
    
    void Application::RenderTile(std::vector<KM::Vertex> &vertices, KM::World *world, BlockPos blockPos, int tile, int layer)
    {
        if (tile == 0 || tile >= BLOCK_COUNT) return;

        if (layer == 0 && (tile == 7 || tile == 8)) {
            return;
        }

        if (layer == 1 && tile != 7 && tile != 8) {
            return;
        }

        float x0 = blockPos.x;
        float y0 = blockPos.y;
        float z0 = blockPos.z;
        float x1 = blockPos.x + 1.0f;
        float y1 = blockPos.y + 1.0f;
        float z1 = blockPos.z + 1.0f;

        BlockDef *block = &BLOCKS_DEFS[tile];

        float bB = 0.5f;
        float bT = 1.0f;
        float bNS = 0.8f;
        float bWE = 0.6f;

        // B

        if (world->getBlockId(blockPos.Down()) == 0 || world->getBlockId(blockPos.Down()) == 7 || (world->getBlockId(blockPos.Down()) == 8 && tile != 8)) {
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

        if (world->getBlockId(blockPos.Up()) == 0 || world->getBlockId(blockPos.Up()) == 7 || (world->getBlockId(blockPos.Up()) == 8 && tile != 8)) {
            int u = ((block->textureIndex[1] % 16) * 16);
            int v = ((block->textureIndex[1] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bT * world->getBlockBrightness(blockPos.Up());

            vertices.push_back({ x1, y1, z1, b, b, b, 1, u1, v1 });
            vertices.push_back({ x1, y1, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x0, y1, z0, b, b, b, 1, u0, v0 });
            vertices.push_back({ x0, y1, z1, b, b, b, 1, u0, v1 });
        }

        // N

        if (world->getBlockId(blockPos.North()) == 0 || world->getBlockId(blockPos.North()) == 7 || (world->getBlockId(blockPos.North()) == 8 && tile != 8)) {
            int u = ((block->textureIndex[2] % 16) * 16);
            int v = ((block->textureIndex[2] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bNS * world->getBlockBrightness(blockPos.North());

            vertices.push_back({ x0, y1, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x1, y1, z0, b, b, b, 1, u0, v0 });
            vertices.push_back({ x1, y0, z0, b, b, b, 1, u0, v1 });
            vertices.push_back({ x0, y0, z0, b, b, b, 1, u1, v1 });
        }

        // S

        if (world->getBlockId(blockPos.South()) == 0 || world->getBlockId(blockPos.South()) == 7 || (world->getBlockId(blockPos.South()) == 8 && tile != 8)) {
            int u = ((block->textureIndex[3] % 16) * 16);
            int v = ((block->textureIndex[3] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bNS * world->getBlockBrightness(blockPos.South());

            vertices.push_back({ x0, y1, z1, b, b, b, 1, u0, v0 });
            vertices.push_back({ x0, y0, z1, b, b, b, 1, u0, v1 });
            vertices.push_back({ x1, y0, z1, b, b, b, 1, u1, v1 });
            vertices.push_back({ x1, y1, z1, b, b, b, 1, u1, v0 });
        }

        // W

        if (world->getBlockId(blockPos.West()) == 0 || world->getBlockId(blockPos.West()) == 7 || (world->getBlockId(blockPos.West()) == 8 && tile != 8)) {
            int u = ((block->textureIndex[4] % 16) * 16);
            int v = ((block->textureIndex[4] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bWE * world->getBlockBrightness(blockPos.West());

            vertices.push_back({ x0, y1, z1, b, b, b, 1, u1, v0 });
            vertices.push_back({ x0, y1, z0, b, b, b, 1, u0, v0 });
            vertices.push_back({ x0, y0, z0, b, b, b, 1, u0, v1 });
            vertices.push_back({ x0, y0, z1, b, b, b, 1, u1, v1 });
        }

        // E

        if (world->getBlockId(blockPos.East()) == 0 || world->getBlockId(blockPos.East()) == 7 || (world->getBlockId(blockPos.East()) == 8 && tile != 8)) {
            int u = ((block->textureIndex[5] % 16) * 16);
            int v = ((block->textureIndex[5] / 16) * 16);
            float u0 = u / 256.0f;
            float v0 = v / 256.0f;
            float u1 = (u + 16.f) / 256.0f;
            float v1 = (v + 16.f) / 256.0f;

            float b = bWE * world->getBlockBrightness(blockPos.East());

            vertices.push_back({ x1, y0, z1, b, b, b, 1, u0, v1 });
            vertices.push_back({ x1, y0, z0, b, b, b, 1, u1, v1 });
            vertices.push_back({ x1, y1, z0, b, b, b, 1, u1, v0 });
            vertices.push_back({ x1, y1, z1, b, b, b, 1, u0, v0 });
        }
    }

    void Application::DrawTexture(int x, int y, int w, int h, int u, int v, int us, int vs, int tw, int th)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(this->m_guiVao);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_guiVbo);

        float u0 = (float) u / (float) tw;
        float v0 = (float) v / (float) th;
        float u1 = (float) (u + us) / (float) tw;
        float v1 = (float) (v + vs) / (float) th;

        const float verts[] =
        {
            x + 0.0f, y + 0.0f, 0.0f,    1, 1, 1, 1,      u0, v0,
            x + 0.0f, y + h + 0.0f, 0.0f,    1, 1, 1, 1,      u0, v1,
            x + w + 0.0f, y + h + 0.0f, 0.0f,    1, 1, 1, 1,      u1, v1,
            x + w + 0.0f, y + h + 0.0f, 0.0f,    1, 1, 1, 1,      u1, v1,
            x + w + 0.0f, y + 0.0f, 0.0f,    1, 1, 1, 1,      u1, v0,
            x + 0.0f, y + 0.0f, 0.0f,    1, 1, 1, 1,      u0, v0
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_BLEND);
        glBindVertexArray(0);
    }

    int Application::CalculateGuiScale(int width, int height) {
       int scale;
        for (scale = 1; scale != 4 && scale < width && scale < height && width / (scale + 1) >= 320 && height / (scale + 1) >= 240; ++scale) {
        }
        return scale;
    }
}