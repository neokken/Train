#include "precomp.h"
#include "TrackDebugger.h"

#include "Input/InputManager.h"
#include "UI/UIManager.h"
#include "Camera/Camera.h"
#include "Renderables/Circle.h"
#include "Renderables/LineSegment.h"

void TrackDebugger::Init( Engine::InputManager* inputManager, TrackManager* trackManager )
{
	m_inputManager = inputManager;
	m_trackManager = trackManager;
}

void TrackDebugger::Update( const Engine::Camera& camera )
{
	const float2 worldPosMouse = camera.GetWorldPosition(m_inputManager->GetMousePos());

	for (const auto& node : std::views::values(m_trackManager->m_nodes))
	{
		float2 diff = node.nodePosition - worldPosMouse;

		if (sqrLength(diff) < NODE_SELECTION_DIST_SQ)
		{
			m_hoveredTrackNode = node.id;
			if (m_inputManager->IsMouseJustDown(GLFW_MOUSE_BUTTON_LEFT))
			{
				if (m_selectedTrackNode != node.id)
				{
					m_selectedTrackNode = node.id;
					m_linkedTrackSegments = CalculateLinkedTrackSegments(node.id);
				}
				else
				{
					m_selectedTrackNode = TrackNodeID::Invalid;
				}

				m_selectedTrackSegment = TrackSegmentID::Invalid;
			}

			m_hoveredTrackSegment = TrackSegmentID::Invalid;
			return;
		}
	}
	m_hoveredTrackNode = TrackNodeID::Invalid;

	for (const auto& segment : std::views::values(m_trackManager->m_segments))
	{
		const TrackNode& nodeA = m_trackManager->GetTrackNode(segment.nodeA);
		const TrackNode& nodeB = m_trackManager->GetTrackNode(segment.nodeB);

		if (SQRDistancePointToSegment(worldPosMouse, nodeA.nodePosition, nodeB.nodePosition) < SEGMENT_SELECTION_DIST_SQ)
		{
			m_hoveredTrackSegment = segment.id;
			if (m_inputManager->IsMouseJustDown(GLFW_MOUSE_BUTTON_LEFT))
			{
				if (m_selectedTrackSegment != segment.id)
				{
					m_selectedTrackSegment = segment.id;
					m_linkedTrackSegments = CalculateLinkedTrackSegments(segment.id);
				}
				else
				{
					m_selectedTrackSegment = TrackSegmentID::Invalid;
				}

				m_selectedTrackNode = TrackNodeID::Invalid;
			}

			m_hoveredTrackNode = TrackNodeID::Invalid;
			return;
		}
	}
	m_hoveredTrackSegment = TrackSegmentID::Invalid;
}

void TrackDebugger::Render( const Engine::Camera& camera, Surface& targetSurface ) const
{
	if (!m_trackManager)
	{
		Engine::Logger::Warn("Invalid TrackNode ID");
	}

	for (const auto& node : std::views::values(m_trackManager->m_nodes))
	{
		uint color = NODE_COLOR_DEFAULT;
		if (node.id == m_hoveredTrackNode) color = NODE_COLOR_HOVER;
		if (node.id == m_selectedTrackNode) color = NODE_COLOR_SELECT;
		if (node.id == m_selectedTrackNode && node.id == m_hoveredTrackNode) color = NODE_COLOR_SELECT_HOVER;

		Engine::Circle::RenderWorldPos(camera, targetSurface, node.nodePosition, NODE_DISPLAY_SIZE, color);
	}

	for (const auto& segment : std::views::values(m_trackManager->m_segments))
	{
		uint color = SEGMENT_COLOR_DEFAULT;
		if (segment.id == m_hoveredTrackSegment) color = SEGMENT_COLOR_HOVER;
		if (segment.id == m_selectedTrackSegment) color = SEGMENT_COLOR_SELECT;
		if (segment.id == m_hoveredTrackSegment && segment.id == m_selectedTrackSegment)
			color = SEGMENT_COLOR_SELECT_HOVER;

		if (m_inputManager->IsKeyDown(GLFW_KEY_LEFT_SHIFT) && (m_selectedTrackSegment != TrackSegmentID::Invalid || m_selectedTrackNode != TrackNodeID::Invalid) && ranges::find(m_linkedTrackSegments, segment.id) != m_linkedTrackSegments.end())
		{
			color = SEGMENT_COLOR_LINKED;
		}

		const TrackNode& a = m_trackManager->GetTrackNode(segment.nodeA);
		const TrackNode& b = m_trackManager->GetTrackNode(segment.nodeB);

		RenderSegment(camera, targetSurface, a.nodePosition, segment.nodeA_Direction, b.nodePosition, segment.nodeB_Direction, 0, color);

		//Engine::LineSegment::RenderWorldPos(camera, targetSurface, a.nodePosition, b.nodePosition, color);
	}
}

void TrackDebugger::UI() const
{
	if (Engine::UIManager::BeginDebugWindow("Track Debugger"))
	{
		// active
		if (ImGui::CollapsingHeader("Selected Nodes", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_selectedTrackNode == TrackNodeID::Invalid && m_selectedTrackSegment == TrackSegmentID::Invalid)
			{
				// ** no selected stuff **
				ImGui::Text("No node or segment selected");
				ImGui::Text("Left click on them to select them.");
			}
			else if (m_selectedTrackNode != TrackNodeID::Invalid)
			{
				// ** selected node **
				ImGui::Text("Selected Node: %d", static_cast<int>(m_selectedTrackNode));

				ImGui::Separator();

				const auto& node = m_trackManager->m_nodes.at(m_selectedTrackNode);
				ImGui::Text("Position: (%.2f, %.2f)", node.nodePosition.x, node.nodePosition.y);
				ImGui::Text("Connections:");

				for (const auto& [segmentID, validConnections] : node.validConnections)
				{
					if (node.connectionLever.at(segmentID) == -1)
					{
						ImGui::Text("Segment %d -> end", segmentID);
						continue;
					}

					const int activeConnection = node.connectionLever.contains(segmentID) ? node.connectionLever.at(segmentID) : -1;

					ImGui::Text("Segment %d -> Segment: %d (selector: %d) >", static_cast<int>(segmentID), static_cast<int>(validConnections.at(activeConnection)), activeConnection);

					ImGui::SameLine();
					for (size_t i = 0; i < validConnections.size(); ++i)
					{
						ImGui::Text("%d", validConnections[i]);
						if (i < validConnections.size() - 1)
						{
							ImGui::SameLine();
							ImGui::Text("|");
							ImGui::SameLine();
						}
					}
				}

				if (node.validConnections.empty())
				{
					ImGui::Text("No in or outgoing connections.");
				}
			}
			else
			{
				// ** selected segment **
				ImGui::Text("Selected Segment: %d", static_cast<int>(m_selectedTrackSegment));
				ImGui::Separator();

				const TrackSegment& segment = m_trackManager->m_segments.at(m_selectedTrackSegment);

				TrackNode nodeA = m_trackManager->GetTrackNode(segment.nodeA);
				TrackNode nodeB = m_trackManager->GetTrackNode(segment.nodeB);

				ImGui::Text("NodeA id: %d (%.2f, %.2f)", static_cast<int>(nodeA.id), nodeA.nodePosition.x, nodeA.nodePosition.y);
				ImGui::Text("NodeB id: %d (%.2f, %.2f)", static_cast<int>(nodeB.id), nodeB.nodePosition.x, nodeB.nodePosition.y);
				ImGui::Text("Segment length: %.2f", segment.distance);
				ImGui::Text("Connections:");

				if (m_linkedTrackSegments.empty())
				{
					ImGui::Text("No outgoing connections.");
				}
				else
				{
					for (const auto& connectedSeg : m_linkedTrackSegments)
					{
						ImGui::Text("SegmentID: %d", static_cast<int>(connectedSeg));
					}
				}
			}
		}

		// **Nodes List**
		if (ImGui::CollapsingHeader("Track Nodes"))
		{
			if (ImGui::BeginTable("NodesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				// Change to 2 columns
				ImGui::TableSetupColumn("ID");
				ImGui::TableSetupColumn("Position");
				ImGui::TableHeadersRow();

				for (const auto& [id, node] : m_trackManager->m_nodes)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%d", static_cast<int>(id));

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("(%.2f, %.2f)", node.nodePosition.x, node.nodePosition.y);
					// Display position in one column
				}
				ImGui::EndTable();
			}
		}

		// **Segments List**
		if (ImGui::CollapsingHeader("Track Segments"))
		{
			if (ImGui::BeginTable("SegmentsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("ID");
				ImGui::TableSetupColumn("Node A");
				ImGui::TableSetupColumn("Node B");
				ImGui::TableSetupColumn("Distance");
				ImGui::TableHeadersRow();

				for (const auto& [id, segment] : m_trackManager->m_segments)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%d", static_cast<int>(id));

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%d", static_cast<int>(segment.nodeA));

					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%d", static_cast<int>(segment.nodeB));

					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%d", static_cast<int>(segment.distance));
				}
				ImGui::EndTable();
			}
		}

		// **Node Inspector**
		static int selectedNodeID = 0;
		ImGui::Separator();
		ImGui::Text("Inspect Node:");
		ImGui::InputInt("Node ID", &selectedNodeID);

		if (m_trackManager->m_nodes.contains(static_cast<TrackNodeID>(selectedNodeID)))
		{
			const auto& node = m_trackManager->m_nodes.at(static_cast<TrackNodeID>(selectedNodeID));
			ImGui::Text("Position: (%.2f, %.2f)", node.nodePosition.x, node.nodePosition.y);
			ImGui::Text("Connections:");
			for (const auto& [segmentID, validConnections] : node.validConnections)
			{
				const int activeConnection = node.connectionLever.contains(segmentID) ? node.connectionLever.at(segmentID) : -1;

				ImGui::Text("Segment %d -> Segment: %d (selector: %d) >", static_cast<int>(segmentID), static_cast<int>(validConnections.at(activeConnection)), activeConnection);

				ImGui::SameLine();
				for (size_t i = 0; i < validConnections.size(); ++i)
				{
					ImGui::Text("%d", validConnections[i]);
					if (i < validConnections.size() - 1)
					{
						ImGui::SameLine();
						ImGui::Text("|");
						ImGui::SameLine();
					}
				}
			}
		}
		else
		{
			ImGui::Text("Invalid Node ID");
		}
	}
	Engine::UIManager::EndDebugWindow();
}

std::vector<TrackSegmentID> TrackDebugger::CalculateLinkedTrackSegments( TrackSegmentID segmentID ) const
{
	std::vector<TrackSegmentID> result;

	const TrackSegment& segment = m_trackManager->GetTrackSegment(segmentID);

	const TrackNode& nodeA = m_trackManager->GetTrackNode(segment.nodeA);
	const TrackNode& nodeB = m_trackManager->GetTrackNode(segment.nodeB);

	const std::vector<TrackSegmentID> nodeALinkedSegments = nodeA.validConnections.contains(segmentID) ? nodeA.validConnections.at(segmentID) : std::vector<TrackSegmentID>();
	const std::vector<TrackSegmentID> nodeBLinkedSegments = nodeB.validConnections.contains(segmentID) ? nodeB.validConnections.at(segmentID) : std::vector<TrackSegmentID>();;

	result.insert(result.end(), nodeALinkedSegments.begin(), nodeALinkedSegments.end());
	result.insert(result.end(), nodeBLinkedSegments.begin(), nodeBLinkedSegments.end());

	return result;
}

std::vector<TrackSegmentID> TrackDebugger::CalculateLinkedTrackSegments( const TrackNodeID nodeID ) const
{
	std::vector<TrackSegmentID> result;
	const TrackNode& node = m_trackManager->GetTrackNode(nodeID);

	for (const auto& segmentList : std::views::values(node.validConnections))
	{
		result.insert(result.end(), segmentList.begin(), segmentList.end());
	}
	return result;
}

void TrackDebugger::RenderSegment( const Engine::Camera& camera, Surface& targetSurface, const float2& pointA, const float2& dirA, const float2& pointB, const float2& dirB, const int segmentCount, const uint color )
{
	float2 startPoint = pointA;
	float2 endPoint = {0};

	const float segmentLength = 1.f / static_cast<float>(segmentCount);

	for (int i = 0; i < segmentCount; i++)
	{
		endPoint = CurveCalculation(pointA, dirA, pointB, dirB, .5f, segmentLength * i);
		Engine::LineSegment::RenderWorldPos(camera, targetSurface, startPoint, endPoint, color);
		startPoint = endPoint;
	}

	endPoint = pointB;

	Engine::LineSegment::RenderWorldPos(camera, targetSurface, startPoint, endPoint, color);
}

float TrackDebugger::SQRDistancePointToSegment( const float2& point, const float2& A, const float2& B )
{
	// thanks chatgpt for this code...

	const float2 AB = B - A;
	const float2 AP = point - A;

	const float AB_lengthSq = sqrLength(AB);
	if (AB_lengthSq == 0.0f)
	{
		// A and B are the same point
		return sqrLength(AP);
	}

	// Project point onto the line (parametric t)
	float t = dot(AP, AB) / AB_lengthSq;
	t = std::clamp(t, 0.0f, 1.0f); // Clamping t to stay within segment

	// Find the closest point on the segment
	const float2 closestPoint = {A.x + t * AB.x, A.y + t * AB.y};

	// Compute distance from point to the closest point
	return sqrLength(point - closestPoint);
}

float2 TrackDebugger::CurveCalculation( const float2& pointA, const float2& dirA, const float2& pointB, const float2& dirB, const float hardness, const float t )
{
	const float2 sMidPointOffset = (pointB - pointA) * dirA;

	const float2 eMidPointOffset = (pointA - pointB) * dirB;

	const float2 startMidPoint = pointA + sMidPointOffset * hardness;
	const float2 endMidPoint = pointB + eMidPointOffset * hardness;

	const float2 linear_SsM = lerp(pointA, startMidPoint, t);
	const float2 linear_sMeM = lerp(startMidPoint, endMidPoint, t);
	const float2 linear_eME = lerp(endMidPoint, pointB, t);
	const float2 square_SM = lerp(linear_SsM, linear_sMeM, t);
	const float2 square_ME = lerp(linear_sMeM, linear_eME, t);

	return lerp(square_SM, square_ME, t);
}
