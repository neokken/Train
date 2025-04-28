#include "precomp.h"
#include "BlockRenderer.h"

#include "SignalManager.h"
#include "TrackManager.h"
#include "Helpers/Renderer.h"
#include "Renderables/CurvedSegment.h"
#include "Renderables/LineSegment.h"

void BlockRenderer::Init( SignalManager& signalManager, TrackManager& trackManager )
{
	m_signalManager = &signalManager;
	m_trackManager = &trackManager;
}

void BlockRenderer::Render( Engine::Camera& camera )
{
	auto blocks = m_signalManager->GetBlockMap();
	for (const auto& block : blocks)
	{
		uint seed = static_cast<uint>(block.first) * 17;
		uint8_t r = 128 + RandomUInt(seed) % 128;
		uint8_t g = 128 + RandomUInt(seed) % 128;
		uint8_t b = 128 + RandomUInt(seed) % 128;
		uint color = (r << 16) | (g << 8) | b;
		for (const auto& connectionList : block.second.connections)
		{
			const Signal& startSig = m_signalManager->GetSignal(connectionList.first);
			const TrackSegment& startSegment = m_trackManager->GetTrackSegment(startSig.segment);
			Engine::CurveData curve{startSegment.nodeA_Position, startSegment.nodeA_Direction, startSegment.nodeB_Position, startSegment.nodeB_Direction};
			float2 lStart = Engine::CurvedSegment::GetPositionOnCurvedSegment(startSig.distanceOnSegment, curve);
			for (const auto& signal : connectionList.second)
			{
				const Signal& sig = m_signalManager->GetSignal(signal);
				const TrackSegment& segment = m_trackManager->GetTrackSegment(sig.segment);
				Engine::CurveData curve2{segment.nodeA_Position, segment.nodeA_Direction, segment.nodeB_Position, segment.nodeB_Direction};
				float2 lEnd = Engine::CurvedSegment::GetPositionOnCurvedSegment(sig.distanceOnSegment, curve2);
				Engine::LineSegment::RenderWorldPos(camera, lStart, lEnd, color, HeightLayer::Debug);
			}
		}
	}
}
