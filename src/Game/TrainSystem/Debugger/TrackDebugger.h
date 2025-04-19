#pragma once
#include "Game/TrainSystem/TrackManager.h"

class TrackRenderer;

namespace Engine
{
	class InputManager;
	class World;
	class Camera;
}

// CONFIGURABLE

constexpr float NODE_DISPLAY_SIZE = 1.f;
constexpr float NODE_SELECTION_DIST = 1.5f;
constexpr float SEGMENT_SELECTION_DIST = 1.f;

constexpr float NODE_SELECTION_DIST_SQ = NODE_SELECTION_DIST * NODE_SELECTION_DIST;
constexpr float SEGMENT_SELECTION_DIST_SQ = SEGMENT_SELECTION_DIST * SEGMENT_SELECTION_DIST;

enum DEBUG_COLORS
{
	NODE_COLOR_DEFAULT = 0x6610F2,
	NODE_COLOR_HOVER = 0x1A8FE3,
	NODE_COLOR_SELECT = 0xF17105,
	NODE_COLOR_SELECT_HOVER = 0xE6C229,

	SEGMENT_COLOR_DEFAULT = 0x6610F2,
	SEGMENT_COLOR_HOVER = 0x1A8FE3,
	SEGMENT_COLOR_SELECT = 0xF17105,
	SEGMENT_COLOR_SELECT_HOVER = 0xE6C229,
	SEGMENT_COLOR_LINKED = 0x8FB339,
};

class TrackDebugger
{
public:
	TrackDebugger() = default;

	void Init( TrackManager* trackManager, TrackRenderer* trackRenderer );

	void Update( const Engine::Camera& camera );

	void Render( const Engine::Camera& camera, Surface& targetSurface ) const;

	void UI();

	void SetVisible( bool value );
	[[nodiscard]] bool GetVisibility() const;

private:
	TrackManager* m_trackManager{nullptr};

	TrackRenderer* m_trackRenderer{nullptr};

	bool m_visible{false};
};
