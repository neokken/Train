#include "precomp.h"
#include "TrackDebugger.h"

#include "Input/InputManager.h"
#include "UI/UIManager.h"
#include "Camera/Camera.h"
#include "Game/TrainSystem/TrackRenderer.h"
#include "Renderables/Circle.h"
#include "Renderables/CurvedSegment.h"
#include "Renderables/LineSegment.h"

void TrackDebugger::Init( TrackManager* trackManager, TrackRenderer* trackRenderer )
{
	m_trackManager = trackManager;
	m_trackRenderer = trackRenderer;
}

void TrackDebugger::Update( const Engine::Camera& camera )
{
	if (!m_visible) return;
}

void TrackDebugger::Render( const Engine::Camera& camera, Surface& targetSurface ) const
{
	if (!m_visible) return;

	if (m_renderConnectedSegments)
	{
		RenderConnectedSegments(camera, targetSurface);
	}
}

void TrackDebugger::UI()
{
	if (!m_visible) return;

	if (Engine::UIManager::BeginDebugWindow("TrackDebugger", &m_visible))
	{
		bool drawDebugTracks = m_trackRenderer->GetTrackRenderer() == TrackRenderType::Debug;

		if (ImGui::Checkbox("Render debugTracks", &drawDebugTracks))
		{
			if (drawDebugTracks)
			{
				m_trackRenderer->SetTrackRenderer(TrackRenderType::Debug);
			}
			else
			{
				m_trackRenderer->SetTrackRenderer(TrackRenderType::Default);
			}
		}

		ImGui::Checkbox("Render connectedSegments", &m_renderConnectedSegments);
	}
	Engine::UIManager::EndDebugWindow();
}

void TrackDebugger::SetVisible( const bool value )
{
	m_visible = value;
}

bool TrackDebugger::GetVisibility() const
{
	return m_visible;
}

void TrackDebugger::RenderConnectedSegments( const Engine::Camera& camera, Surface& targetSurface ) const
{
	auto ks = std::views::keys(m_trackManager->GetSegmentMap());
	std::set<TrackSegmentID> allSegments{ks.begin(), ks.end()};

	const std::vector<uint> colors = {0x6388b4, 0xffae34, 0xef6f6a, 0x8cc2ca, 0x55ad89, 0xc3bc3f, 0xbb7683, 0xbaa094};
	int colorIndex = -1;

	std::queue<TrackSegmentID> openList;

	while (!allSegments.empty())
	{
		openList.push(*allSegments.begin());
		allSegments.erase(allSegments.begin());
		colorIndex++;
		colorIndex %= colors.size();

		while (!openList.empty())
		{
			TrackSegmentID c = openList.front();
			openList.pop();

			const TrackSegment& tc = m_trackManager->GetTrackSegment(c);

			const TrackNode& nodeA = m_trackManager->GetTrackNode(tc.nodeA);
			const TrackNode& nodeB = m_trackManager->GetTrackNode(tc.nodeB);

			for (const auto& a : nodeA.validConnections.at(c))
			{
				if (allSegments.contains(a))
				{
					allSegments.erase(a);
					openList.push(a);
				}
			}
			for (const auto& a : nodeB.validConnections.at(c))
			{
				if (allSegments.contains(a))
				{
					allSegments.erase(a);
					openList.push(a);
				}
			}

			Engine::CurveData data{tc.nodeA_Position, tc.nodeA_Direction, tc.nodeB_Position, tc.nodeB_Direction};
			Engine::CurvedSegment::RenderTrackLinesWorldPos(camera, targetSurface, data, colors.at(colorIndex), 0.f);
		}
	}
}
