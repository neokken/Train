#include "precomp.h"
#include "TrackBuilder.h"

#include "TrackManager.h"
#include "TrackRenderer.h"
#include "Camera/Camera.h"
#include "Helpers/Renderer.h"
#include "Input/InputManager.h"
#include "Renderables/Arrow.h"
#include "Renderables/Circle.h"
#include "Renderables/CurvedSegment.h"
#include "Renderables/LineSegment.h"

void TrackBuilder::Init( TrackManager& trackManager, TrackRenderer& trackRenderer, SignalManager& signalManager )
{
	m_trackManager = &trackManager;
	m_trackRenderer = &trackRenderer;
	m_signalManager = &signalManager;
}

void TrackBuilder::Update( Engine::Camera& camera, [[maybe_unused]] float deltaTime )
{
	const Engine::InputManager& input = Input::get();

	if ((
			input.IsKeyJustDown(GLFW_KEY_B) && (m_currentProgress == BuildProgress::StartNode || m_currentProgress == BuildProgress::FirstNodeFinished))
		|| input.IsKeyJustDown(GLFW_KEY_N) && (m_currentProgress == BuildProgress::Signal)
		|| input.IsKeyJustDown(GLFW_KEY_ESCAPE))
	{
		camera.SetBuildMode(false);
		m_currentProgress = BuildProgress::NoBuild;
		m_hoveredSegment = TrackSegmentID::Invalid;
		return;
	}

	if (m_currentProgress == BuildProgress::NoBuild || m_currentProgress == BuildProgress::Signal)
	{
		if (input.IsKeyJustDown(GLFW_KEY_B))
		{
			camera.SetBuildMode(true);
			m_currentProgress = BuildProgress::StartNode;
		}
	}
	if (m_currentProgress == BuildProgress::NoBuild || m_currentProgress == BuildProgress::StartNode || m_currentProgress == BuildProgress::FirstNodeFinished)
	{
		if (input.IsKeyJustDown(GLFW_KEY_N))
		{
			camera.SetBuildMode(true);
			m_currentProgress = BuildProgress::Signal;
		}
	}
	if (m_currentProgress == BuildProgress::NoBuild) return;

	// handle deletion
	m_hoveredSegment = TrackSegmentID::Invalid;
	const float2 worldMouse = camera.GetWorldPosition(input.GetMousePos());
	if (m_currentProgress == BuildProgress::StartNode || m_currentProgress == BuildProgress::FirstNodeFinished)
	{
		for (const auto& segment : std::views::values(m_trackManager->GetSegmentMap()))
		{
			Engine::CurveData curveData{segment.nodeA_Position, segment.nodeA_Direction, segment.nodeB_Position, segment.nodeB_Direction};

			const float distance = sqrLength(Engine::CurvedSegment::GetClosestPoint(curveData, worldMouse) - worldMouse);

			if (distance < 1.f)
			{
				m_hoveredSegment = segment.id;
				break;
			}
		}
	}
	else if (m_currentProgress == BuildProgress::Signal)
	{
		m_hoveredSignal = m_signalManager->FindClosestSignal(worldMouse, 2.f, true);
	}

	if (input.IsKeyDown(GLFW_KEY_X) || input.IsKeyDown(GLFW_KEY_DELETE))
	{
		if ((m_currentProgress == BuildProgress::StartNode || m_currentProgress == BuildProgress::FirstNodeFinished)
			&& m_hoveredSegment != TrackSegmentID::Invalid)
		{
			m_signalManager->RemoveSegmentSignals(m_hoveredSegment);
			m_trackManager->DeleteTrackPart(m_hoveredSegment);
			m_hoveredSegment = TrackSegmentID::Invalid;
		}
		if (m_currentProgress == BuildProgress::Signal && m_hoveredSignal != SignalID::Invalid)
		{
			m_signalManager->RemoveSignal(m_hoveredSignal);
			m_hoveredSignal = SignalID::Invalid;
		}
	}

	if (input.IsMouseClicked(GLFW_MOUSE_BUTTON_RIGHT))
	{
		switch (m_currentProgress)
		{
		case BuildProgress::NoBuild:
			break;
		case BuildProgress::StartNode:
			camera.SetBuildMode(false);
			m_currentProgress = BuildProgress::NoBuild;
			break;
		case BuildProgress::FirstNodeFinished:
			m_currentProgress = BuildProgress::StartNode;
			break;
		case BuildProgress::Signal:
			camera.SetBuildMode(false);
			m_currentProgress = BuildProgress::NoBuild;
			break;
		}
	}

	if (m_currentProgress == BuildProgress::StartNode)
	{
		UpdateTempNode(camera, false);

		if (input.IsMouseClicked(GLFW_MOUSE_BUTTON_LEFT))
		{
			// validate node??

			m_nodeA = m_tempNode;

			m_currentProgress = BuildProgress::FirstNodeFinished;
		}
	}

	else if (m_currentProgress == BuildProgress::FirstNodeFinished)
	{
		UpdateTempNode(camera, true);

		if (input.IsMouseClicked(GLFW_MOUSE_BUTTON_LEFT))
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

			m_currentProgress = BuildProgress::StartNode;
		}
	}

	else if (m_currentProgress == BuildProgress::Signal)
	{
		UpdateTempSignal(camera);

		if (input.IsMouseClicked(GLFW_MOUSE_BUTTON_LEFT) && m_hoveredSignal == SignalID::Invalid)
		{
			if (m_tempSignal.connectedSegment != TrackSegmentID::Invalid)
				m_signalManager->BuildSignal(m_tempSignal.connectedSegment, m_tempSignal.segmentDistance, m_tempSignal.directionTowardsNodeB, m_tempSignal.signalType, m_tempSignal.connectedSignal);
		}
	}
}

void TrackBuilder::Render( const Engine::Camera& camera ) const
{
	if (m_currentProgress != BuildProgress::NoBuild)
	{
		const std::unordered_map<TrackNodeID, TrackNode>& nodes = m_trackManager->GetNodeMap();

		for (const auto& node : std::views::values(nodes))
		{
			Engine::Circle::RenderWorldPos(camera, node.nodePosition, .75f, GetColor(Color::TrackNodeInfo), 0.f, 10);
			Engine::Circle::RenderWorldPos(camera, node.nodePosition, .75f, GetColor(Color::TrackNodeInfo), 0.f, 4);
		}

		if (m_hoveredSegment != TrackSegmentID::Invalid)
		{
			TrackRenderer::RenderTrackSegment(camera, m_trackManager->GetTrackSegment(m_hoveredSegment), TrackRenderType::RailsOnly, Color::TrackNodeInfo);
		}
		if (m_hoveredSignal != SignalID::Invalid)
		{
			const Signal& signal = m_signalManager->GetSignal(m_hoveredSignal);
			RenderSignal(camera, {float2(0, 0), signal.segment, SignalID::Invalid, signal.distanceOnSegment, signal.directionTowardsNodeB, signal.type}, GetColor(Color::TrackNodeInfo));
			
		}
		if (m_currentProgress == BuildProgress::StartNode)
		{
			RenderNode(camera, m_tempNode, DEFAULT_COLOR, false);
		}
		else if (m_currentProgress == BuildProgress::FirstNodeFinished)
		{
			const Engine::CurveData data{m_nodeA.trackNodePosition, m_nodeA.directionFloat2, m_tempNode.trackNodePosition, -m_tempNode.directionFloat2};

			if (bool valid_curve = Engine::CurvedSegment::CheckCurveValidity(data, m_buildStrictness)) //, &camera, &renderTarget))
			{
				RenderSegment(camera, m_nodeA, m_tempNode, Color::TrackRail);
			}
			else
			{
				RenderSegment(camera, m_nodeA, m_tempNode, Color::TrackSelectedInvalid);
			}

			RenderNode(camera, m_nodeA, DEFAULT_COLOR, true);
			RenderNode(camera, m_tempNode, DEFAULT_COLOR, false);
		}
		else if (m_currentProgress == BuildProgress::Signal && m_hoveredSignal == SignalID::Invalid)
		{
			const Color color = (m_tempSignal.connectedSegment != TrackSegmentID::Invalid) ? Color::SignalSelected : Color::SignalInvalid;
			RenderSignal(camera, m_tempSignal, GetColor(color));
			if (m_tempSignal.connectedSignal != SignalID::Invalid)
			{
				const Signal& signal = m_signalManager->GetSignal(m_tempSignal.connectedSignal);
				RenderSignal(camera, {float2(0.f), signal.segment, SignalID::Invalid, signal.distanceOnSegment, signal.directionTowardsNodeB, signal.type}, 0x95F6EC);
			}
		}
	}
}

void TrackBuilder::UpdateTempNode( const Engine::Camera& camera, const bool isSecondNode )
{
	const Engine::InputManager& input = Input::get();

	const float2 worldMousePosition = camera.GetWorldPosition(input.GetMousePos());

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

		if (input.IsKeyUp(GLFW_KEY_LEFT_SHIFT) && input.IsKeyUp(GLFW_KEY_LEFT_CONTROL))
		{
			if (input.GetScrollDelta() > 0.f)
			{
				m_tempNode.trackSegmentSelector = (m_tempNode.trackSegmentSelector + 1) % static_cast<int>(hoverNode.validConnections.size());
			}
			else if (input.GetScrollDelta() < 0.f)
			{
				m_tempNode.trackSegmentSelector = (m_tempNode.trackSegmentSelector + static_cast<int>(hoverNode.validConnections.size()) - 1) % static_cast<int>(hoverNode.validConnections.size());
			}
		}

		if (input.IsKeyDown(GLFW_KEY_LEFT_SHIFT) && input.IsKeyUp(GLFW_KEY_LEFT_CONTROL))
		{
			if (input.GetScrollDelta() > 0.f)
			{
				m_tempNode.direction = static_cast<TrackDirection>((static_cast<int>(m_tempNode.direction) + 1) % static_cast<int>(TrackDirection::Count));
			}
			else if (input.GetScrollDelta() < 0.f)
			{
				m_tempNode.direction = static_cast<TrackDirection>((static_cast<int>(m_tempNode.direction) - 1 + static_cast<int>(TrackDirection::Count)) % static_cast<int>(TrackDirection::Count));
			}

			m_tempNode.trackSegmentId = TrackSegmentID::Invalid;
			m_tempNode.directionFloat2 = toFloat2(m_tempNode.direction);
		}

		if (input.IsKeyUp(GLFW_KEY_LEFT_SHIFT) && input.IsKeyUp(GLFW_KEY_LEFT_CONTROL))
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

		if (input.IsKeyDown(GLFW_KEY_LEFT_ALT))
		{
			m_tempNode.trackNodePosition = worldMousePosition;
		}
		else
		{
			m_tempNode.trackNodePosition = float2(round(worldMousePosition.x / 1.f), round(worldMousePosition.y / 1.f));
		}

		if (input.IsKeyUp(GLFW_KEY_LEFT_CONTROL))
		{
			if (input.GetScrollDelta() > 0.f)
			{
				m_tempNode.direction = static_cast<TrackDirection>((static_cast<int>(m_tempNode.direction) + 1) % static_cast<int>(TrackDirection::Count));
			}
			else if (input.GetScrollDelta() < 0.f)
			{
				m_tempNode.direction = static_cast<TrackDirection>((static_cast<int>(m_tempNode.direction) - 1 + static_cast<int>(TrackDirection::Count)) % static_cast<int>(TrackDirection::Count));
			}

			m_tempNode.directionFloat2 = toFloat2(m_tempNode.direction);
		}
	}
}

void TrackBuilder::UpdateTempSignal( const Engine::Camera& camera )
{
	const Engine::InputManager& input = Input::get();

	if (input.IsKeyUp(GLFW_KEY_LEFT_CONTROL))
	{
		if (input.GetScrollDelta() > 0)
		{
			switch (m_tempSignal.signalType)
			{
			case SignalType::Default:
				m_tempSignal.signalType = SignalType::Chain;
				break;
			case SignalType::Chain:
				m_tempSignal.signalType = SignalType::Default;
				break;
			}
		}
		else if (input.GetScrollDelta() < 0)
		{
			switch (m_tempSignal.signalType)
			{
			case SignalType::Default:
				m_tempSignal.signalType = SignalType::Chain;
				break;
			case SignalType::Chain:
				m_tempSignal.signalType = SignalType::Default;
				break;
			}
		}
	}

	const float2 worldMousePosition = camera.GetWorldPosition(input.GetMousePos());

	TrackSegmentID hoverSegment = m_trackManager->GetClosestTrackSegment(worldMousePosition, 30);
	m_tempSignal.connectedSegment = hoverSegment;
	if (hoverSegment != TrackSegmentID::Invalid)
	{
		const TrackSegment& hoverSeg = m_trackManager->GetTrackSegment(hoverSegment);
		float distance = m_trackManager->GetClosestDistanceOnTrackSegment(hoverSegment, worldMousePosition);

		bool spotOccupied = false;
		for (auto signal : hoverSeg.signals)
		{
			const Signal& sig = m_signalManager->GetSignal(signal);
			if (abs(sig.distanceOnSegment - distance) < SIGNAL_SNAPPING * 2.f / hoverSeg.distance)
			{
				//2nd dir placing
				Engine::CurveData curve = {hoverSeg.nodeA_Position, hoverSeg.nodeA_Direction, hoverSeg.nodeB_Position, hoverSeg.nodeB_Direction};
				float2 trackPos = Engine::CurvedSegment::GetPositionOnCurvedSegment(distance, curve);
				float2 trackDir = Engine::CurvedSegment::GetDirectionOnCurvedSegment(distance, curve);
				float2 trackDirRight = float2(-trackDir.y, trackDir.x);
				float2 mouseDir = normalize(worldMousePosition - trackPos);
				spotOccupied = true;
				if ((dot(mouseDir, trackDirRight) > 0) != sig.directionTowardsNodeB)
				{
					m_tempSignal.segmentDistance = sig.distanceOnSegment;
					m_tempSignal.directionTowardsNodeB = !sig.directionTowardsNodeB;
					m_tempSignal.connectedSignal = sig.id;
				}
				else
				{
					m_tempSignal.connectedSignal = SignalID::Invalid;
					m_tempSignal.connectedSegment = TrackSegmentID::Invalid;
					m_tempSignal.basePosition = worldMousePosition;
				}

			}
		}
		if (!spotOccupied)
		{
			m_tempSignal.connectedSignal = SignalID::Invalid;
			if (!input.IsKeyDown(GLFW_KEY_LEFT_SHIFT))
			{
				distance *= hoverSeg.distance;
				distance = round(distance / SIGNAL_SNAPPING) * SIGNAL_SNAPPING;
				distance /= hoverSeg.distance;
			}

			m_tempSignal.segmentDistance = distance;
			Engine::CurveData curve = {hoverSeg.nodeA_Position, hoverSeg.nodeA_Direction, hoverSeg.nodeB_Position, hoverSeg.nodeB_Direction};
			float2 trackPos = Engine::CurvedSegment::GetPositionOnCurvedSegment(distance, curve);
			float2 trackDir = Engine::CurvedSegment::GetDirectionOnCurvedSegment(distance, curve);
			float2 trackDirRight = float2(-trackDir.y, trackDir.x);
			float2 mouseDir = normalize(worldMousePosition - trackPos);
			if (dot(mouseDir, trackDirRight) > 0)
			{
				m_tempSignal.directionTowardsNodeB = true;
			}
			else
			{
				m_tempSignal.directionTowardsNodeB = false;
			}
		}
	}
	else
	{
		m_tempSignal.basePosition = worldMousePosition;
	}
}

void TrackBuilder::RenderNode( const Engine::Camera& camera, const TrackBuildData& data, uint colorNode, const bool onlyShowArrow ) const
{
	const float2 dir = data.directionFloat2;

	Engine::Arrow::RenderWorldPos(camera, data.trackNodePosition, data.directionFloat2, .5f, colorNode);

	if (!onlyShowArrow)
	{
		const float2 left = data.trackNodePosition + float2(-dir.y, dir.x) * -0.75f;
		const float2 right = data.trackNodePosition + float2(-dir.y, dir.x) * 0.75f;

		const float2 leftS = data.trackNodePosition + float2(-dir.y, dir.x) * -0.70f;
		const float2 rightS = data.trackNodePosition + float2(-dir.y, dir.x) * 0.70f;

		Engine::LineSegment::RenderWorldPos(camera, left + dir * .5f, left + dir * -.5f, colorNode);
		Engine::LineSegment::RenderWorldPos(camera, right + dir * .5f, right + dir * -.5f, colorNode);

		Engine::LineSegment::RenderWorldPos(camera, leftS + dir * .5f, leftS + dir * -.5f, colorNode);
		Engine::LineSegment::RenderWorldPos(camera, rightS + dir * .5f, rightS + dir * -.5f, colorNode);
	}

	if (data.trackSegmentId != TrackSegmentID::Invalid)
	{
		TrackRenderer::RenderTrackSegment(camera, m_trackManager->GetTrackSegment(data.trackSegmentId), TrackRenderType::RailsOnly, Color::TrackSelected);
	}
}

void TrackBuilder::RenderSignal( const Engine::Camera& camera, const SignalBuildData& data, uint color ) const
{
	if (data.connectedSegment != TrackSegmentID::Invalid)
	{
		const TrackSegment& segment = m_trackManager->GetTrackSegment(data.connectedSegment);
		Engine::CurveData curve = {segment.nodeA_Position, segment.nodeA_Direction, segment.nodeB_Position, segment.nodeB_Direction};
		float2 pos = Engine::CurvedSegment::GetPositionOnCurvedSegment(data.segmentDistance, curve);
		float2 dir = Engine::CurvedSegment::GetDirectionOnCurvedSegment(data.segmentDistance, curve);
		float2 placeDir = data.directionTowardsNodeB ? float2(-dir.y, dir.x) : float2(dir.y, -dir.x);
		switch (data.signalType)
		{
		case SignalType::Default:
			{
				pos += placeDir * 1.75f;
				Engine::Renderer::GetRenderer().DrawLineRectangle(float3(camera.GetCameraPosition(pos), HeightLayer::Tracks + 1), dir, float2(1.1f, 1.1f) * camera.GetZoomLevel(), Engine::RGB8ToRGB32(color), 0.08f * camera.GetZoomLevel());
				break;
			}
		case SignalType::Chain:
			{
				pos += placeDir * 1.7f;
				Engine::Circle::RenderWorldPos(camera, pos, 0.55f, color, 0.08f);
				break;
			}
		}
	}
	else
	{
		float2 pos = data.basePosition;
		switch (data.signalType)
		{
		case SignalType::Default:
			{
				Engine::Renderer::GetRenderer().DrawLineRectangle(float3(camera.GetCameraPosition(pos), HeightLayer::Tracks + 1), float2(0, 1), float2(1.1f, 1.1f) * camera.GetZoomLevel(), Engine::RGB8ToRGB32(color), 0.08f * camera.GetZoomLevel());
				break;
			}
		case SignalType::Chain:
			{
				Engine::Circle::RenderWorldPos(camera, pos, 0.55f, color, 0.08f);
				break;
			}
		}
	}
}

void TrackBuilder::RenderSegment( const Engine::Camera& camera, const TrackBuildData& nodeA, const TrackBuildData& nodeB, const Color trackColor )
{
	const TrackSegment seg(nodeA.trackNodePosition, nodeB.trackNodePosition, nodeA.directionFloat2, -nodeB.directionFloat2);

	if (abs(nodeA.trackNodePosition.x - nodeB.trackNodePosition.x) < .01f && abs(nodeA.trackNodePosition.y - nodeB.trackNodePosition.y) < .01f &&
		abs(nodeA.directionFloat2.x - nodeB.directionFloat2.x) < .01f && abs(nodeA.directionFloat2.y - nodeB.directionFloat2.y) < .01f)
		return;

	TrackRenderer::RenderTrackSegment(camera, seg, TrackRenderType::RailsOnly, trackColor);
}
