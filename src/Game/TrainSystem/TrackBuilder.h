#pragma once
#include "SignalManager.h"
#include "Game/TrainSystem/TrackSegment.h"

class TrackRenderer;
class TrackDebugger;
class TrackManager;

constexpr uint INVALID_COLOR = 0xFF2E00;
constexpr uint DEFAULT_COLOR = 0xFFFECB;
constexpr uint CONNECT_COLOR = 0x417B5A;
constexpr float SIGNAL_SNAPPING = 0.5f;

namespace Engine
{
	class Camera;
	class InputManager;
}

class TrackBuilder
{
public:
	TrackBuilder() = default;

	void Init( TrackManager& trackManager, TrackRenderer& trackRenderer, SignalManager& signalManager );

	void Update( Engine::Camera& camera, float deltaTime );

	void Render( const Engine::Camera& camera ) const;

	[[nodiscard]] bool InBuildMode() const { return (m_currentProgress != BuildProgress::NoBuild); }

private:
	TrackManager* m_trackManager{nullptr};
	TrackRenderer* m_trackRenderer{nullptr};
	SignalManager* m_signalManager{nullptr};

	struct TrackBuildData
	{
		float2 trackNodePosition;
		float2 directionFloat2{1.f, 0.f};

		TrackNodeID trackNodeID{TrackNodeID::Invalid};
		TrackSegmentID trackSegmentId;
		int trackSegmentSelector{0};
		TrackDirection direction{TrackDirection::E};
	};

	struct SignalBuildData
	{
		float2 basePosition;
		TrackSegmentID connectedSegment{TrackSegmentID::Invalid};
		SignalID connectedSignal{SignalID::Invalid};
		float segmentDistance{0.f};
		bool directionTowardsNodeB;
		SignalType signalType{SignalType::Default};
	};

	TrackBuildData m_tempNode;

	TrackBuildData m_nodeA;
	SignalBuildData m_tempSignal;

	float m_buildStrictness = .5f;

	TrackSegmentID m_hoveredSegment = TrackSegmentID::Invalid; // for deleting 
	SignalID m_hoveredSignal = SignalID::Invalid;

	enum class BuildProgress : std::uint8_t
	{
		NoBuild,
		Signal,
		StartNode,
		FirstNodeFinished,
	} m_currentProgress{BuildProgress::NoBuild};

private:
	void UpdateTempNode( const Engine::Camera& camera, bool isSecondNode );
	void UpdateTempSignal( const Engine::Camera& camera );

	void RenderNode( const Engine::Camera& camera, const TrackBuildData& data, uint colorNode, bool onlyShowArrow ) const;
	void RenderSignal( const Engine::Camera& camera, const SignalBuildData& data, uint color ) const;

	static void RenderSegment( const Engine::Camera& camera, const TrackBuildData& nodeA, const TrackBuildData& nodeB, Color trackColor );
};
