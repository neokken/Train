namespace Engine
{
	class Renderer
	{
		static Renderer& GetRenderer();

	private:
		Renderer();
		static Renderer* m_instance;
	};
}
