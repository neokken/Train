#include "precomp.h"
#include "TrackBuilder.h"

#include "TrackManager.h"
#include "TrackRenderer.h"
#include "Camera/Camera.h"
#include "Debugger/TrackDebugger.h"
#include "Input/InputManager.h"
#include "Renderables/Arrow.h"
#include "Renderables/Circle.h"
#include "Renderables/CurvedSegment.h"
#include "Renderables/LineSegment.h"

void TrackBuilder::Init( Engine::InputManager* inputManager, TrackManager* trackManager, TrackRenderer* trackRenderer )
{
	m_inputManager = inputManager;
	m_trackManager = trackManager;
	m_trackRenderer = trackRenderer;
}

void TrackBuilder::Update( const Engine::Camera& camera, [[maybe_unused]] float deltaTime )
{
	if (m_currentProgress == BuildProgress::NoBuild)
	{
		if (m_inputManager->IsKeyJustDown(GLFW_KEY_B))
		{
			m_currentProgress = BuildProgress::Start;
		}
		return;
	}

	if (m_inputManager->IsKeyJustDown(GLFW_KEY_B))
	{
		m_currentProgress = BuildProgress::NoBuild;
		return;
	}

	if (m_currentProgress == BuildProgress::Start)
	{
		UpdateTempNode(camera, false);

		if (m_inputManager->IsMouseJustDown(GLFW_MOUSE_BUTTON_LEFT))
		{
			// validate node??

			m_nodeA = m_tempNode;

			m_currentProgress = BuildProgress::FirstNodeFinished;
		}
	}

	else if (m_currentProgress == BuildProgress::FirstNodeFinished)
	{
		UpdateTempNode(camera, true);

		if (m_inputManager->IsMouseJustDown(GLFW_MOUSE_BUTTON_LEFT))
		{
			// validate node??
			Engine::CurveData curveData{m_nodeA.trackNodePosition, m_nodeA.directionFloat2, m_tempNode.trackNodePosition, -m_tempNode.directionFloat2};
			const bool valid_curve = Engine::CurvedSegment::CheckCurveValidity(curveData, m_buildStrictness); //, &camera, &renderTarget)

			if (!valid_curve) return;

			if (m_nodeA.trackSegmentId != TrackSegmentID::Invalid)
			{
				if (m_tempNode.trackSegmentId != TrackSegmentID::Invalid)
				{
					m_trackManager->BuildTrackPart(m_nodeA.trackNodePosition, TrackDirection::Empty, m_nodeA.trackSegmentId, m_tempNode.trackNodePosition, TrackDirection::Empty, m_tempNode.trackSegmentId);
				}
				else
				{
					m_trackManager->BuildTrackPart(m_nodeA.trackNodePosition, TrackDirection::Empty, m_nodeA.trackSegmentId, m_tempNode.trackNodePosition, m_tempNode.direction, TrackSegmentID::Invalid);
				}
			}
			else
			{
				if (m_tempNode.trackSegmentId != TrackSegmentID::Invalid)
				{
					m_trackManager->BuildTrackPart(m_nodeA.trackNodePosition, m_nodeA.direction, TrackSegmentID::Invalid, m_tempNode.trackNodePosition, TrackDirection::Empty, m_tempNode.trackSegmentId);
				}
				else
				{
					m_trackManager->BuildTrackPart(m_nodeA.trackNodePosition, m_nodeA.direction, TrackSegmentID::Invalid, m_tempNode.trackNodePosition, m_tempNode.direction, TrackSegmentID::Invalid);
				}
			}

			m_currentProgress = BuildProgress::Start;
		}
	}
}

void TrackBuilder::Render( const Engine::Camera& camera, Surface& renderTarget ) const
{
	if (m_currentProgress != BuildProgress::NoBuild)
	{
		const std::unordered_map<TrackNodeID, TrackNode>& nodes = m_trackManager->GetNodeMap();

		for (const auto& node : std::views::values(nodes))
		{
			Engine::Circle::RenderWorldPos(camera, renderTarget, node.nodePosition, .75f, GetColor(Color::TrackNodeInfo), 10);
			Engine::Circle::RenderWorldPos(camera, renderTarget, node.nodePosition, .75f, GetColor(Color::TrackNodeInfo), 4);
		}
	}

	if (m_currentProgress == BuildProgress::Start)
	{
		RenderNode(camera, renderTarget, m_tempNode, DEFAULT_COLOR, CONNECT_COLOR, false);
	}

	if (m_currentProgress == BuildProgress::FirstNodeFinished)
	{
		const Engine::CurveData data{m_nodeA.trackNodePosition, m_nodeA.directionFloat2, m_tempNode.trackNodePosition, -m_tempNode.directionFloat2};

		if (bool valid_curve = Engine::CurvedSegment::CheckCurveValidity(data, m_buildStrictness)) //, &camera, &renderTarget))
		{
			RenderSegment(camera, renderTarget, m_nodeA, m_tempNode, Color::TrackRail);
		}
		else
		{
			RenderSegment(camera, renderTarget, m_nodeA, m_tempNode, Color::TrackSelectedInvalid);
		}

		RenderNode(camera, renderTarget, m_nodeA, DEFAULT_COLOR, CONNECT_COLOR, true);
		RenderNode(camera, renderTarget, m_tempNode, DEFAULT_COLOR, CONNECT_COLOR, false);
	}
}

void TrackBuilder::UpdateTempNode( const Engine::Camera& camera, const bool isSecondNode )
{
	const float2 worldMousePosition = camera.GetWorldPosition(m_inputManager->GetMousePos());

	const TrackNodeID hoverNodeID = m_trackManager->GetNodeByPosition(worldMousePosition, 1.5f);

	if (hoverNodeID != TrackNodeID::Invalid)
	{
		if (hoverNodeID != m_tempNode.trackNodeID)
		{
			m_tempNode.trackSegmentSelector = 0;
		}

		m_tempNode.trackNodeID = hoverNodeID;

		const TrackNode& hoverNode = m_trackManager->GetTrackNode(hoverNodeID);
		m_tempNode.trackNodePosition = hoverNode.nodePosition;

		if (isSecondNode && m_nodeA.trackNodeID == hoverNodeID && m_nodeA.trackNodeID != TrackNodeID::Invalid)
		{
			m_tempNode.trackNodeID = TrackNodeID::Invalid;
			return;
		}

		if (m_inputManager->IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			if (m_inputManager->GetScrollDelta() > 0.f)
			{
				m_tempNode.trackSegmentSelector = (m_tempNode.trackSegmentSelector + 1) % static_cast<int>(hoverNode.validConnections.size());
			}
			else if (m_inputManager->GetScrollDelta() < 0.f)
			{
				m_tempNode.trackSegmentSelector = (m_tempNode.trackSegmentSelector + static_cast<int>(hoverNode.validConnections.size()) - 1) % static_cast<int>(hoverNode.validConnections.size());
			}
		}

		if (m_inputManager->IsKeyDown(GLFW_KEY_LEFT_CONTROL))
		{
			if (m_inputManager->GetScrollDelta() > 0.f)
			{
				m_tempNode.direction = static_cast<TrackDirection>((static_cast<int>(m_tempNode.direction) + 1) % static_cast<int>(TrackDirection::Count));
			}
			else if (m_inputManager->GetScrollDelta() < 0.f)
			{
				m_tempNode.direction = static_cast<TrackDirection>((static_cast<int>(m_tempNode.direction) - 1 + static_cast<int>(TrackDirection::Count)) % static_cast<int>(TrackDirection::Count));
			}

			m_tempNode.trackSegmentId = TrackSegmentID::Invalid;
			m_tempNode.directionFloat2 = toFloat2(m_tempNode.direction);
		}

		if (m_inputManager->IsKeyDown(GLFW_KEY_LEFT_SHIFT) || m_inputManager->IsKeyUp(GLFW_KEY_LEFT_CONTROL))
		{
			std::vector<TrackSegmentID> keys;
			for (const auto& key : views::keys(hoverNode.validConnections))
			{
				keys.push_back(key);
			}

			m_tempNode.trackSegmentId = keys[m_tempNode.trackSegmentSelector];

			const TrackSegment& selectedSegment = m_trackManager->GetTrackSegment(m_tempNode.trackSegmentId);

			if (selectedSegment.nodeA == hoverNodeID)
			{
				m_tempNode.directionFloat2 = -selectedSegment.nodeA_Direction;
			}
			else
			{
				m_tempNode.directionFloat2 = -selectedSegment.nodeB_Direction;
			}

			if (isSecondNode)
			{
				m_tempNode.directionFloat2 = -m_tempNode.directionFloat2;
			}

			m_tempNode.direction = toTrackDir(m_tempNode.directionFloat2);
		}
	}
	else
	{
		m_tempNode.trackSegmentId = TrackSegmentID::Invalid;
		m_tempNode.trackNodeID = TrackNodeID::Invalid;

		if (m_inputManager->IsKeyDown(GLFW_KEY_LEFT_ALT))
		{
			m_tempNode.trackNodePosition = worldMousePosition;
		}
		else
		{
			m_tempNode.trackNodePosition = float2(round(worldMousePosition.x / 1.f), round(worldMousePosition.y / 1.f));
		}

		if (m_inputManager->IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			if (m_inputManager->GetScrollDelta() > 0.f)
			{
				m_tempNode.direction = static_cast<TrackDirection>((static_cast<int>(m_tempNode.direction) + 1) % static_cast<int>(TrackDirection::Count));
			}
			else if (m_inputManager->GetScrollDelta() < 0.f)
			{
				m_tempNode.direction = static_cast<TrackDirection>((static_cast<int>(m_tempNode.direction) - 1 + static_cast<int>(TrackDirection::Count)) % static_cast<int>(TrackDirection::Count));
			}

			m_tempNode.directionFloat2 = toFloat2(m_tempNode.direction);
		}
	}
}

void TrackBuilder::RenderNode( const Engine::Camera& camera, Surface& renderTarget, const TrackBuildData& data, uint colorNode, uint colorConnectedSegment, const bool onlyShowArrow ) const
{
	const float2 dir = data.directionFloat2;

	Engine::Arrow::RenderWorldPos(camera, renderTarget, data.trackNodePosition, data.directionFloat2, .5f, colorNode);

	if (!onlyShowArrow)
	{
		const float2 left = data.trackNodePosition + float2(-dir.y, dir.x) * -0.75f;
		const float2 right = data.trackNodePosition + float2(-dir.y, dir.x) * 0.75f;

		const float2 leftS = data.trackNodePosition + float2(-dir.y, dir.x) * -0.70f;
		const float2 rightS = data.trackNodePosition + float2(-dir.y, dir.x) * 0.70f;

		Engine::LineSegment::RenderWorldPos(camera, renderTarget, left + dir * .5f, left + dir * -.5f, colorNode);
		Engine::LineSegment::RenderWorldPos(camera, renderTarget, right + dir * .5f, right + dir * -.5f, colorNode);

		Engine::LineSegment::RenderWorldPos(camera, renderTarget, leftS + dir * .5f, leftS + dir * -.5f, colorNode);
		Engine::LineSegment::RenderWorldPos(camera, renderTarget, rightS + dir * .5f, rightS + dir * -.5f, colorNode);
	}

	if (data.trackSegmentId != TrackSegmentID::Invalid)
	{
		TrackRenderer::RenderTrackSegment(camera, renderTarget, m_trackManager->GetTrackSegment(data.trackSegmentId), TrackRenderType::RailsOnly, Color::TrackSelected);
	}
}

void TrackBuilder::RenderSegment( const Engine::Camera& camera, Surface& renderTarget, const TrackBuildData& nodeA, const TrackBuildData& nodeB, const Color trackColor )
{
	const TrackSegment seg(nodeA.trackNodePosition, nodeB.trackNodePosition, nodeA.directionFloat2, -nodeB.directionFloat2);

	if (abs(nodeA.trackNodePosition.x - nodeB.trackNodePosition.x) < .01f && abs(nodeA.trackNodePosition.y - nodeB.trackNodePosition.y) < .01f &&
		abs(nodeA.directionFloat2.x - nodeB.directionFloat2.x) < .01f && abs(nodeA.directionFloat2.y - nodeB.directionFloat2.y) < .01f)
		return;

	TrackRenderer::RenderTrackSegment(camera, renderTarget, seg, TrackRenderType::RailsOnly, trackColor);
}
