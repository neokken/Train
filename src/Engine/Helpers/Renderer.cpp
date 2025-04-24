#include "precomp.h"
#include "Renderer.h"

namespace Engine
{
	Renderer* Renderer::m_instance{nullptr};

	void Renderer::Render()
	{
		glViewport(0, 0, SCRWIDTH, SCRHEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glBindVertexArray(m_lineVAO);

		float3 clear = RGB8ToRGB32(GetColor(Color::BackGround));
		glClearColor(clear.x, clear.y, clear.z, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		//                   width            {xyz},{rgb}
		std::vector<std::pair<float, std::vector<float>>> lineVertices;
		for (auto line : m_queuedLines) // Might be slow but probably fine
		{
			bool inserted = false;
			for (auto& size : lineVertices)
			{
				if (size.first == line.width) // NOLINT(clang-diagnostic-float-equal)
				{
					size.second.push_back(line.startPos.x);
					size.second.push_back(line.startPos.y);
					size.second.push_back(-line.startPos.z);
					size.second.push_back(line.color.x);
					size.second.push_back(line.color.y);
					size.second.push_back(line.color.z);
					size.second.push_back(line.endPos.x);
					size.second.push_back(line.endPos.y);
					size.second.push_back(-line.endPos.z);
					size.second.push_back(line.color.x);
					size.second.push_back(line.color.y);
					size.second.push_back(line.color.z);
					inserted = true;
				}
			}
			if (!inserted)
			{
				float3 start = line.startPos;
				float3 end = line.endPos;
				lineVertices.push_back(std::pair<float, std::vector<float>>(line.width, {start.x, start.y, -start.z, line.color.x, line.color.y, line.color.z, end.x, end.y, -end.z, line.color.x, line.color.y, line.color.z}));
			}
		}

		for (auto line : lineVertices)
		{
			if (line.first <= 0.f)
			{
				m_lineShader->Bind();
				m_lineShader->SetInputMatrix("MVP", view);
			}
			else
			{
				m_rectangleShader->Bind();
				m_rectangleShader->SetInputMatrix("MVP", view);
				m_rectangleShader->SetFloat("width", line.first);
				m_rectangleShader->SetFloat2("resolution", float2(SCRWIDTH, SCRHEIGHT));
			}

			//Draw lines
			glBindVertexArray(m_lineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * line.second.size(), line.second.data(), GL_DYNAMIC_DRAW);



			glDrawArrays(GL_LINES, 0, static_cast<int>(line.second.size() / 2));
		}

		m_queuedLines.clear();
	}

	Renderer& Engine::Renderer::GetRenderer()
	{
		if (m_instance == nullptr)
		{
			m_instance = new Renderer();
		}
		return *m_instance;
	}

	void Renderer::DrawLine( const Line& line )
	{
		m_queuedLines.push_back(line);
	}

	void Renderer::DrawRectangle( const float3& position, const float2 direction, const float2 halfSize, const float3 color )
	{
		float3 posA = position + float3(direction, 0.f) * halfSize.y;
		float3 posB = position - float3(direction, 0.f) * halfSize.y;
		float width = halfSize.x * 2.f;
		m_queuedLines.push_back({posA, posB, color, width});
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	GLTexture& Renderer::GetRenderTexture()
	{
		return *m_renderTexture;
	}

	Renderer::Renderer()
	{
		float nearPlane = 0.0f;
		float farPlane = 1000.0f;
		view = mat4::Orthographic(float2(0), SCRWIDTH, SCRHEIGHT, nearPlane, farPlane);

		m_lineShader = new Shader("./assets/shaders/line.vert", "./assets/shaders/line.frag", false);
		m_rectangleShader = new Shader("./assets/shaders/line.vert", "./assets/shaders/line.frag", "./assets/shaders/rectangle.geom");

		//Set up VBO
		glGenBuffers(1, &m_lineVBO);
		glGenVertexArrays(1, &m_lineVAO);

		glBindVertexArray(m_lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);

		// Set up the vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // positions

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // colors

		//Set up framebuffer for blurring
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		//Create a texture
		m_renderTexture = new GLTexture(SCRWIDTH, SCRHEIGHT);

		//Bind the texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTexture->ID, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			Logger::Critical("Framebuffer not complete: {}\n", status);
	}
}
