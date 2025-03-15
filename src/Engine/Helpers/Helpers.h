#pragma once
namespace Engine {
	inline bool MouseOverUI() {
		ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureKeyboard || io.WantCaptureMouse;
	}
}