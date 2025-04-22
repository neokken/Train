#include "precomp.h"
#include "Renderer.h"

namespace Engine
{
	Renderer* Renderer::m_instance{nullptr};

	void Renderer::Render()
	{
		static const float3 screenWidthReciprical = float3(float2(1 / float2(SCRWIDTH, SCRHEIGHT)), 1.f) * 2.f;

		m_lineShader->Bind();
		m_lineShader->SetInputMatrix("MVP", mat4::Identity());
		glViewport(0, 0, SCRWIDTH, SCRHEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glBindVertexArray(m_lineVAO);

		float3 clear = RGB8ToRGB32(GetColor(Color::BackGround));
		glClearColor(clear.x, clear.y, clear.z, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		//                   width            {xyz},{rgb}
		std::vector<std::pair<float, std::vector<float3>>> lineVertices;
		for (auto line : m_queuedLines) // Might be slow but probably fine
		{
			bool inserted = false;
			for (auto& size : lineVertices)
			{
				if (size.first == line.width) // NOLINT(clang-diagnostic-float-equal)
				{
					size.second.insert(size.second.end(), {line.startPos * screenWidthReciprical - float3(1), line.color, line.endPos * screenWidthReciprical - float3(1), line.color});
					inserted = true;
				}
			}
			if (!inserted)
			{
				lineVertices.push_back(std::pair<float, std::vector<float3>>(line.width, {line.startPos * screenWidthReciprical - float3(1), line.color, line.endPos * screenWidthReciprical - float3(1), line.color}));
			}
		}

		for (auto line : lineVertices)
		{
			//Draw lines
			glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * line.second.size(), line.second.data(), GL_DYNAMIC_DRAW);

			// Set up the vertex attributes
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // positions

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // colors

			glLineWidth(line.first);
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

	// ReSharper disable once CppMemberFunctionMayBeConst
	GLTexture& Renderer::GetRenderTexture()
	{
		return *m_renderTexture;
	}

	Renderer::Renderer()
	{
		m_lineShader = new Shader("./assets/shaders/line.vert", "./assets/shaders/line.frag", false);

		//Set up VBO
		glGenBuffers(1, &m_lineVBO);
		glGenVertexArrays(1, &m_lineVAO);

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
