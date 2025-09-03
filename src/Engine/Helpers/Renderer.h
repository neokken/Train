namespace Engine
{
	struct Line
	{
		float3 startPos, endPos;
		float3 color;
		float width = 0.f;
	};

	class Renderer
	{
	public:
		void Render();
		static Renderer& GetRenderer();
		void DrawLine( const Line& line );

		/**
		 * Draws a line in the form of a rectangle
		 * @param direction normalized direction
		 * @param size halfSize, y is aligned with direction
		 */
		void DrawRectangle( const float3& position, float2 direction, float2 halfSize, float3 color );

		void DrawLineRectangle( float3 pos, float2 dir, float2 size, float3 color, float width = 0.f );
		GLTexture& GetRenderTexture();
	private:
		Renderer();
		static Renderer* m_instance;

		std::vector<Line> m_queuedLines;

		mat4 view;
		uint m_FBO{0};
		uint m_lineVBO{0};
		uint m_lineVAO{0};
		GLTexture* m_renderTexture{nullptr};
		Shader* m_lineShader;
		Shader* m_rectangleShader;
	};
}
