#include "precomp.h"
#include "TrackDebugger.h"

#include "Input/InputManager.h"
#include "UI/UIManager.h"
#include "Camera/Camera.h"
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
		float2 diff = node.m_nodePosition - worldPosMouse;

		if (sqrLength(diff) < NODE_SELECTION_DIST_SQ)
		{
			m_hoveredTrackNode = node.m_id;
			if (m_inputManager->IsMouseJustDown(GLFW_MOUSE_BUTTON_LEFT))
			{
				if (m_selectedTrackNode != node.m_id)
				{
					m_selectedTrackNode = node.m_id;
					m_linkedTrackSegments = CalculateLinkedTrackSegments(node.m_id);
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
		const TrackNode& nodeA = m_trackManager->GetTrackNode(segment.m_nodeA);
		const TrackNode& nodeB = m_trackManager->GetTrackNode(segment.m_nodeB);

		if (SQRDistancePointToSegment(worldPosMouse, nodeA.m_nodePosition, nodeB.m_nodePosition) < SEGMENT_SELECTION_DIST_SQ)
		{
			m_hoveredTrackSegment = segment.m_id;
			if (m_inputManager->IsMouseJustDown(GLFW_MOUSE_BUTTON_LEFT))
			{
				if (m_selectedTrackSegment != segment.m_id)
				{
					m_selectedTrackSegment = segment.m_id;
					m_linkedTrackSegments = CalculateLinkedTrackSegments(segment.m_id);
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
		if (node.m_id == m_hoveredTrackNode) color = NODE_COLOR_HOVER;
		if (node.m_id == m_selectedTrackNode) color = NODE_COLOR_SELECT;
		if (node.m_id == m_selectedTrackNode && node.m_id == m_hoveredTrackNode) color = NODE_COLOR_SELECT_HOVER;

		DrawCircle(camera, targetSurface, node.m_nodePosition, NODE_DISPLAY_SIZE, color);
	}

	for (const auto& segment : std::views::values(m_trackManager->m_segments))
	{
		uint color = SEGMENT_COLOR_DEFAULT;
		if (segment.m_id == m_hoveredTrackSegment) color = SEGMENT_COLOR_HOVER;
		if (segment.m_id == m_selectedTrackSegment) color = SEGMENT_COLOR_SELECT;
		if (segment.m_id == m_hoveredTrackSegment && segment.m_id == m_selectedTrackSegment)
			color = SEGMENT_COLOR_SELECT_HOVER;

		if (m_inputManager->IsKeyDown(GLFW_KEY_LEFT_SHIFT) && (m_selectedTrackSegment != TrackSegmentID::Invalid || m_selectedTrackNode != TrackNodeID::Invalid) && ranges::find(m_linkedTrackSegments, segment.m_id) != m_linkedTrackSegments.end())
		{
			color = SEGMENT_COLOR_LINKED;
		}

		const TrackNode& a = m_trackManager->GetTrackNode(segment.m_nodeA);
		const TrackNode& b = m_trackManager->GetTrackNode(segment.m_nodeB);

		Engine::LineSegment::RenderWorldPos(camera, targetSurface, a.m_nodePosition, b.m_nodePosition, color);
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
				ImGui::Text("Position: (%.2f, %.2f)", node.m_nodePosition.x, node.m_nodePosition.y);
				ImGui::Text("Connections:");
				for (const auto& [segmentID, validConnections] : node.m_validConnections)
				{
					const int activeConnection = node.m_connectionLever.contains(segmentID) ? node.m_connectionLever.at(segmentID) : -1;

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

				if (node.m_validConnections.empty())
				{
					ImGui::Text("No outgoing connections.");
				}
			}
			else
			{
				// ** selected segment **
				ImGui::Text("Selected Segment: %d", static_cast<int>(m_selectedTrackSegment));
				ImGui::Separator();

				const TrackSegment& segment = m_trackManager->m_segments.at(m_selectedTrackSegment);

				TrackNode nodeA = m_trackManager->GetTrackNode(segment.m_nodeA);
				TrackNode nodeB = m_trackManager->GetTrackNode(segment.m_nodeB);

				ImGui::Text("NodeA id: %d (%.2f, %.2f)", static_cast<int>(nodeA.m_id), nodeA.m_nodePosition.x, nodeA.m_nodePosition.y);
				ImGui::Text("NodeB id: %d (%.2f, %.2f)", static_cast<int>(nodeB.m_id), nodeB.m_nodePosition.x, nodeB.m_nodePosition.y);

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
					ImGui::Text("(%.2f, %.2f)", node.m_nodePosition.x, node.m_nodePosition.y);
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
					ImGui::Text("%d", static_cast<int>(segment.m_nodeA));

					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%d", static_cast<int>(segment.m_nodeB));
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
			ImGui::Text("Position: (%.2f, %.2f)", node.m_nodePosition.x, node.m_nodePosition.y);
			ImGui::Text("Connections:");
			for (const auto& [segmentID, validConnections] : node.m_validConnections)
			{
				const int activeConnection = node.m_connectionLever.contains(segmentID) ? node.m_connectionLever.at(segmentID) : -1;

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

	const TrackNode& nodeA = m_trackManager->GetTrackNode(segment.m_nodeA);
	const TrackNode& nodeB = m_trackManager->GetTrackNode(segment.m_nodeB);

	const std::vector<TrackSegmentID> nodeALinkedSegments = nodeA.m_validConnections.contains(segmentID) ? nodeA.m_validConnections.at(segmentID) : std::vector<TrackSegmentID>();
	const std::vector<TrackSegmentID> nodeBLinkedSegments = nodeB.m_validConnections.contains(segmentID) ? nodeB.m_validConnections.at(segmentID) : std::vector<TrackSegmentID>();;

	result.insert(result.end(), nodeALinkedSegments.begin(), nodeALinkedSegments.end());
	result.insert(result.end(), nodeBLinkedSegments.begin(), nodeBLinkedSegments.end());

	return result;
}

std::vector<TrackSegmentID> TrackDebugger::CalculateLinkedTrackSegments( const TrackNodeID nodeID ) const
{
	std::vector<TrackSegmentID> result;
	const TrackNode& node = m_trackManager->GetTrackNode(nodeID);

	for (const auto& segmentList : std::views::values(node.m_validConnections))
	{
		result.insert(result.end(), segmentList.begin(), segmentList.end());
	}
	return result;
}

void TrackDebugger::DrawCircle( const Engine::Camera& camera, Surface& targetSurface, const float2& center, const float circleSize, const uint color, const int segmentCount )
{
	const float segmentCountF = static_cast<float>(segmentCount);

	for (int i = 0; i < segmentCount; i++)
	{
		const float iF = static_cast<float>(i);

		const float angle1 = (2.0f * PI * iF) / segmentCountF;
		const float angle2 = (2.0f * PI * (iF + 1.f)) / segmentCountF;

		float2 p1 = center + float2{circleSize * cos(angle1), circleSize * sin(angle1)};
		float2 p2 = center + float2{circleSize * cos(angle2), circleSize * sin(angle2)};

		Engine::LineSegment::RenderWorldPos(camera, targetSurface, p1, p2, color);
	}
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
