#pragma once

namespace Engine {
	inline bool MouseOverUI() {
		ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureKeyboard || io.WantCaptureMouse;
	}
}

struct ScopedTimer {
	Timer t;
	const char* name;

	ScopedTimer( const char* funcName ) {
		name = funcName;
	}

	~ScopedTimer();
};

#define PROFILE_FUNCTION() ScopedTimer timer(__FUNCTION__)