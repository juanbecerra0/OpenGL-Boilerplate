#include "TestCube.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {
    TestCube::TestCube()
        : m_Proj(glm::perspective(90.0f, 1.33333f, 0.1f, 1000.0f)), 
          m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
          m_Translation(0, 0, -120), m_Scale(1, 1, 1), m_Rotation(0, 0, 0)
	{
        /* Define vertex buffer */

        // x, y, z, s, t
        float positions[] = {
            // Front face
            -50.0f, -50.0f, -50.0f, 0.0f, 0.0f, // 0
             50.0f, -50.0f, -50.0f, 1.0f, 0.0f, // 1
             50.0f,  50.0f, -50.0f, 1.0f, 1.0f, // 2
            -50.0f,  50.0f, -50.0f, 0.0f, 1.0f, // 3

            // Left face
            -50.0f, -50.0f,  50.0f, 0.0f, 0.0f, // 4
            -50.0f, -50.0f, -50.0f, 1.0f, 0.0f, // 5
            -50.0f,  50.0f, -50.0f, 1.0f, 1.0f, // 6
            -50.0f,  50.0f,  50.0f, 0.0f, 1.0f, // 7

            // Right face
             50.0f, -50.0f, -50.0f, 0.0f, 0.0f, // 8
             50.0f, -50.0f,  50.0f, 1.0f, 0.0f, // 9
             50.0f,  50.0f,  50.0f, 1.0f, 1.0f, // 10
             50.0f,  50.0f, -50.0f, 0.0f, 1.0f, // 11

            // Top face
            -50.0f,  50.0f, -50.0f, 0.0f, 0.0f, // 12
             50.0f,  50.0f, -50.0f, 1.0f, 0.0f, // 13
             50.0f,  50.0f,  50.0f, 1.0f, 1.0f, // 14
            -50.0f,  50.0f,  50.0f, 0.0f, 1.0f, // 15

            // Bottom face
            -50.0f, -50.0f,  50.0f, 0.0f, 0.0f, // 16
             50.0f, -50.0f,  50.0f, 1.0f, 0.0f, // 17
             50.0f, -50.0f, -50.0f, 1.0f, 1.0f, // 18
            -50.0f, -50.0f, -50.0f, 0.0f, 1.0f, // 19

            // Back face
             50.0f, -50.0f,  50.0f, 0.0f, 0.0f, // 20
            -50.0f, -50.0f,  50.0f, 1.0f, 0.0f, // 21
            -50.0f,  50.0f,  50.0f, 1.0f, 1.0f, // 22
             50.0f,  50.0f,  50.0f, 0.0f, 1.0f  // 23
        };

        unsigned int indicies[] = {
            // Front face (0 - 3)
            0, 1, 2,
            2, 3, 0,

            // Left face (4 - 7)
            4, 5, 6,
            6, 7, 4,

            // Right face (8 - 11)
            8, 9, 10,
            10, 11, 8,

            // Top face (12 - 15)
            12, 13, 14,
            14, 15, 12,

            // Bottom face (16 - 19)
            16, 17, 18,
            18, 19, 16,

            // Back face (20 - 23)
            20, 21, 22,
            22, 23, 20
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_VAO = std::make_unique<VertexArray>();
        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 5 * 24 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(3);  // x, y, z
        layout.Push<float>(2);  // s, t

        m_VAO -> AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 6 * 6);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader->Bind();
        m_Texture = std::make_unique<Texture>("res/textures/wood.jpg");
        m_Shader -> SetUniform1i("u_Texture", 0);
	}
	TestCube::~TestCube()
	{

	}
	void TestCube::OnUpdate(float deltaTime)
	{

	}
	void TestCube::OnRender()
	{
		GLCall(glClearColor(0.4f, 0.4f, 0.4f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
        GLCall(glCullFace(GL_BACK));

        Renderer renderer;

        m_Texture->Bind();

        {
            // Translate
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);

            // Rotate
            model = glm::rotate(model, m_Rotation.x, glm::vec3(1, 0, 0));
            model = glm::rotate(model, m_Rotation.y, glm::vec3(0, 1, 0));
            model = glm::rotate(model, m_Rotation.z, glm::vec3(0, 0, 1));

            // Scale
            model = glm::scale(model, m_Scale);

            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader -> Bind();
            m_Shader -> SetUniformMat4f("u_MVP", mvp);

            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
	}
	void TestCube::OnImGuiRender()
	{
        ImGui::SliderFloat3("Translation", &m_Translation.x, -500.0f, 500.0f);
        ImGui::SliderFloat3("Scale", &m_Scale.x, 0.0f, 4.0f);
        ImGui::SliderFloat3("Rotate", &m_Rotation.x, 0.0f, 6.28319f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}