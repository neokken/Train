#include "precomp.h"
#include "TrackRenderer.h"

#include "TrackManager.h"
#include "Renderables/CurvedSegment.h"

void TrackRenderer::Init( TrackManager* trackManager )
{
	m_trackManager = trackManager;
}

void TrackRenderer::Render( const Engine::Camera& camera ) const
{
	const std::unordered_map<TrackSegmentID, TrackSegment>& segments = m_trackManager->GetSegmentMap();

	for (const auto& segment : std::views::values(segments))
	{
		RenderTrackSegment(camera, segment, m_currentTrackRenderer);
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
