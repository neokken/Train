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


		static bool BeginGameplayWindow( const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0 );
		static bool BeginDebugWindow( const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0 );

		static void EndGameplayWindow();
		static void EndDebugWindow();

		static UIManagerSettings GetSettings();
		static void SetSettings( UIManagerSettings settings );

	private:
		static UIManagerSettings settings;
		static int2 m_mainWindowPos;
		static int2 m_mainWindowSize;
	};
}
