#pragma once
#include "Game/TrainSystem/TrackSegment.h"

class TrackRenderer;
class TrackDebugger;
class TrackManager;

constexpr uint INVALID_COLOR = 0xFF2E00;
constexpr uint DEFAULT_COLOR = 0xFFFECB;
constexpr uint CONNECT_COLOR = 0x417B5A;

namespace Engine
{
	class Camera;
	class InputManager;
}

class TrackBuilder
{
public:
	TrackBuilder() = default;

	void Init( TrackManager* trackManager, TrackRenderer* trackRenderer );

	void Update( Engine::Camera& camera, float deltaTime );

	void Render( const Engine::Camera& camera, Surface& renderTarget ) const;

	[[nodiscard]] bool InBuildMode() const { return (m_currentProgress != BuildProgress::NoBuild); }

private:
	TrackManager* m_trackManager{nullptr};
	TrackRenderer* m_trackRenderer{nullptr};

	struct TrackBuildData
	{
		float2 trackNodePosition;
		float2 directionFloat2{1.f, 0.f};

		TrackNodeID trackNodeID{TrackNodeID::Invalid};
		TrackSegmentID trackSegmentId;
		int trackSegmentSelector{0};
		TrackDirection direction{TrackDirection::E};
	};

	TrackBuildData m_tempNode;

	TrackBuildData m_nodeA;

	float m_buildStrictness = .5f;

	TrackSegmentID m_hoveredSegment = TrackSegmentID::Invalid; // for deleting 

	enum class BuildProgress : std::uint8_t
	{
		NoBuild,
		Start,
		FirstNodeFinished,
	} m_currentProgress{BuildProgress::NoBuild};

private:
	void UpdateTempNode( const Engine::Camera& camera, bool isSecondNode );

	void RenderNode( const Engine::Camera& camera, Surface& renderTarget, const TrackBuildData& data, uint colorNode, bool onlyShowArrow ) const;

	static void RenderSegment( const Engine::Camera& camera, Surface& renderTarget, const TrackBuildData& nodeA, const TrackBuildData& nodeB, Color trackColor );
};
