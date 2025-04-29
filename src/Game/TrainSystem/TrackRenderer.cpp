#include "precomp.h"
#include "TrackRenderer.h"

#include "SignalManager.h"
#include "TrackManager.h"
#include "Camera/Camera.h"
#include "Helpers/Renderer.h"
#include "Renderables/Circle.h"
#include "Renderables/CurvedSegment.h"

void TrackRenderer::Init( TrackManager& trackManager, SignalManager& signalManager )
{
	m_trackManager = &trackManager;
	m_signalManager = &signalManager;
}

void TrackRenderer::Render( const Engine::Camera& camera ) const
{
	const std::unordered_map<TrackSegmentID, TrackSegment>& segments = m_trackManager->GetSegmentMap();

	for (const auto& segment : std::views::values(segments))
	{
		RenderTrackSegment(camera, segment, m_currentTrackRenderer);
	}
	const auto& signals = m_signalManager->GetSignalMap();
	for (const auto& signal : std::views::values(signals))
	{
		RenderTrackSignal(camera, signal);
	}
}

void TrackRenderer::SetTrackRenderer( const TrackRenderType type )
{
	m_currentTrackRenderer = type;
}

TrackRenderType TrackRenderer::GetTrackRenderer() const
{
	return m_currentTrackRenderer;
}

void TrackRenderer::RenderTrackSegment( const Engine::Camera& camera, const TrackSegment& segment, const TrackRenderType type, const Color trackColor, const Color SpokeColor )
{
	const Engine::CurveData data{segment.nodeA_Position, segment.nodeA_Direction, segment.nodeB_Position, segment.nodeB_Direction};

	switch (type)
	{
	case TrackRenderType::Default:

		Engine::CurvedSegment::RenderTrackWorldPos(camera, data, GetColor(trackColor), GetColor(SpokeColor), .75f, .875f, .1f, .2f, 1.f, .1f);
	//{
	//	Engine::CurvedSegment a(segment.nodeA_Position, segment.nodeB_Position, segment.nodeA_Direction, .segment.nodeB_Direction, .5f, 0, 10);
	//	a.RenderBezierPoints(camera, targetSurface);
	//}

		break;
	case TrackRenderType::RailsOnly:

		Engine::CurvedSegment::RenderTrackLinesWorldPos(camera, data, GetColor(trackColor), .75f);

		Engine::CurvedSegment::RenderTrackLinesWorldPos(camera, data, GetColor(trackColor), .65f);

		break;
	case TrackRenderType::Debug:

		Engine::CurvedSegment::RenderTrackLinesWorldPos(camera, data, GetColor(trackColor), .75f);
		break;
	}
}

void TrackRenderer::RenderTrackSignal( const Engine::Camera& camera, const Signal& signal ) const
{
	const TrackSegment& segment = m_trackManager->GetTrackSegment(signal.segment);
	Engine::CurveData curve{segment.nodeA_Position, segment.nodeA_Direction, segment.nodeB_Position, segment.nodeB_Direction};
	float2 pos = Engine::CurvedSegment::GetPositionOnCurvedSegment(signal.distanceOnSegment, curve);

	float2 dir = Engine::CurvedSegment::GetDirectionOnCurvedSegment(signal.distanceOnSegment, curve);
	float2 placeDir = signal.directionTowardsNodeB ? float2(-dir.y, dir.x) : float2(dir.y, -dir.x);

	Color lightColor = Color::SignalIndicator_Go;
	if (signal.blockInFront == SignalBlockID::Invalid || m_signalManager->GetBlock(signal.blockInFront).containingTrain != TrainID::Invalid) lightColor = Color::SignalIndicator_AllBlocked;

	switch (signal.type)
	{
	case SignalType::Default:
		{
			pos += placeDir * 1.75f;
			Engine::Renderer::GetRenderer().DrawLineRectangle(float3(camera.GetCameraPosition(pos), HeightLayer::Tracks + 1), dir, float2(1.1f, 1.1f) * camera.GetZoomLevel(), Engine::RGB8ToRGB32(GetColor(Color::Signal_Default)), 0.08f * camera.GetZoomLevel());
			Engine::Renderer::GetRenderer().DrawRectangle(float3(camera.GetCameraPosition(pos), HeightLayer::Tracks + 2), dir, float2(0.35f, 0.35f) * camera.GetZoomLevel(), Engine::RGB8ToRGB32(GetColor(lightColor)));
			break;
		}
	case SignalType::Chain:
		{
			pos += placeDir * 1.7f;
			Engine::Circle::RenderWorldPos(camera, pos, 0.55f, GetColor(Color::Signal_Chain), 0.08f);

			Engine::Renderer::GetRenderer().DrawRectangle(float3(camera.GetCameraPosition(pos), HeightLayer::Tracks + 2), dir, float2(0.2f, 0.2f) * camera.GetZoomLevel(), Engine::RGB8ToRGB32(GetColor(lightColor)));
			break;
		}
	}
}
