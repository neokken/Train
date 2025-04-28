#pragma once
#include "Game/TrainSystem/SignalManager.h"
#include "Game/TrainSystem/TrackManager.h"

class TrackRenderer;

namespace Engine
{
	class InputManager;
	class World;
	class Camera;
}

// CONFIGURABLE

constexpr float NODE_SELECTION_DIST = 1.f;
constexpr float SEGMENT_SELECTION_DIST = 1.f;
constexpr float SIGNAL_SELECTION_DIST = 1.f;

class TrackDebugger
{
public:
	TrackDebugger() = default;

	void Init( TrackManager& trackManager, TrackRenderer& trackRenderer, SignalManager& signalManager );

	void Update( const Engine::Camera& camera );

	void Render( const Engine::Camera& camera ) const;

	void UI();

	void SetVisible( bool value );
	[[nodiscard]] bool GetVisibility() const;

	void EnableSelectMode( bool value );
	[[nodiscard]] bool GetSelectMode() const;

private:
	void SetHoverNode( TrackNodeID id );
	void SetHoverSegment( TrackSegmentID id );

	void SetSelectNode( TrackNodeID id );
	void SetSelectSegment( TrackSegmentID id );

	void SegmentInfo( TrackSegmentID segmentID );

	void NodeSegmentInfo( TrackSegmentID segmentID, bool nodeA );

	void NodeInfo( TrackNodeID nodeID );

	void RenderConnectedSegments( const Engine::Camera& camera ) const;
	void RenderSignal( const Engine::Camera& camera, SignalID signalID ) const;

	TrackManager* m_trackManager{nullptr};

	TrackRenderer* m_trackRenderer{nullptr};
	SignalManager* m_signalManager{nullptr};

	bool m_visible{false};

	bool m_renderConnectedSegments{false};

	bool m_selectMode{false};

	bool m_hoverSafety{false}; // if being set by ImGui is will be set. so there is a frame delay were it can be set again by update

	TrackNodeID m_hoveredNode{TrackNodeID::Invalid};
	TrackSegmentID m_hoveredSegment{TrackSegmentID::Invalid};
	SignalID m_hoveredSignal{SignalID::Invalid};

	TrackNodeID m_selectedNode{TrackNodeID::Invalid};
	TrackSegmentID m_selectedSegment{TrackSegmentID::Invalid};

	SignalBlockID m_selectedBlock{SignalBlockID::Invalid};
	SignalID m_selectedSignal{SignalID::Invalid};
};
