#pragma once

namespace Engine
{
	struct UIManagerSettings
	{
		bool debugMode = true;
		bool allowMainWindowStretching = false;
	};

	class UIManager
	{
	public:
		static void Init();
		static void StartFrame();
		static void Render();
		static void DrawMainWindow( uint texture );
		static int2 GetMainWindowCursorPos( const int2& mousePos );
		static UIManagerSettings settings;

	private:
		static int2 m_mainWindowPos;
		static int2 m_mainWindowSize;
	};
}
