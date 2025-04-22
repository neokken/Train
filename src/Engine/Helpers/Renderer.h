namespace Engine
{
	struct Line
	{
		float3 startPos, endPos;
		float3 color;
		float width = 1.f;
	};

	class Renderer
	{
	public:
		void Render();
		static Renderer& GetRenderer();
		void DrawLine( const Line& line );
		GLTexture& GetRenderTexture();
	private:
		Renderer();
		static Renderer* m_instance;

		std::vector<Line> m_queuedLines;

		uint m_FBO{0};
		uint m_lineVBO{0};
		uint m_lineVAO{0};
		GLTexture* m_renderTexture{nullptr};
		Shader* m_lineShader;
	};
}
