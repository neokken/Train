#pragma once
#include "Game/TrainSystem/TrackSegment.h"

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

	void Init( Engine::InputManager* inputManager, TrackManager* trackManager, TrackDebugger* trackDebugger );

	void Update( const Engine::Camera& camera, float deltaTime );

	void Render( const Engine::Camera& camera, Surface& renderTarget ) const;

private:
	Engine::InputManager* m_inputManager{nullptr};
	TrackManager* m_trackManager{nullptr};
	TrackDebugger* m_trackDebugger{nullptr};

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

	enum class BuildProgress : std::uint8_t
	{
		NoBuild,
		Start,
		FirstNodeFinished,

		SecondNodeFinished,
	} m_currentProgress{BuildProgress::NoBuild};

private:
	void UpdateTempNode( const Engine::Camera& camera, bool isSecondNode );

	void RenderNode( const Engine::Camera& camera, Surface& renderTarget, const TrackBuildData& data, uint colorNode, uint colorConnectedSegment ) const;

	static void RenderSegment( const Engine::Camera& camera, Surface& renderTarget, const TrackBuildData& nodeA, const TrackBuildData& nodeB, uint trackColor );
};
