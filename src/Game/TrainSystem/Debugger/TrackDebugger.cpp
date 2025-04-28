#include "precomp.h"
#include "TrackDebugger.h"

#include "Input/InputManager.h"
#include "UI/UIManager.h"
#include "Camera/Camera.h"
#include "Game/TrainSystem/TrackRenderer.h"
#include "Renderables/Circle.h"
#include "Renderables/CurvedSegment.h"
#include "Renderables/LineSegment.h"

void TrackDebugger::Init( TrackManager& trackManager, TrackRenderer& trackRenderer, SignalManager& signalManager )
{
	m_trackManager = &trackManager;
	m_trackRenderer = &trackRenderer;
	m_signalManager = &signalManager;
}

void TrackDebugger::Update( const Engine::Camera& camera )
{
	if (!m_visible)
	{
		return;
	}

	if (m_selectMode)
	{
		if (m_hoverSafety)
		{
			m_hoverSafety = false;
			return;
		}

		float distanceNode;
		float distanceSegment;

		const float2 worldMousePosition = camera.GetWorldPosition(Input::get().GetMousePos());

		m_hoveredNode = m_trackManager->GetNodeByPosition(worldMousePosition, NODE_SELECTION_DIST, &distanceNode);
		m_hoveredSegment = m_trackManager->GetSegmentByPosition(worldMousePosition, SEGMENT_SELECTION_DIST, &distanceSegment);

		if (distanceNode < distanceSegment + .25f) // we add .25f since now we give nodes a bit more priority otherwise they are hard to select
		{
			m_hoveredSegment = TrackSegmentID::Invalid;
		}
		else
		{
			m_hoveredNode = TrackNodeID::Invalid;
		}

		if (Input::get().IsMouseJustDown(GLFW_MOUSE_BUTTON_LEFT))
		{
			if (m_hoveredNode != TrackNodeID::Invalid)
			{
				m_selectedNode = m_selectedNode == m_hoveredNode ? TrackNodeID::Invalid : m_hoveredNode;
				m_selectedSegment = TrackSegmentID::Invalid;
			}

			if (m_hoveredSegment != TrackSegmentID::Invalid)
			{
				m_selectedSegment = m_selectedSegment == m_hoveredSegment ? TrackSegmentID::Invalid : m_hoveredSegment;
				m_selectedNode = TrackNodeID::Invalid;
			}
		}
	}
}

void TrackDebugger::Render( const Engine::Camera& camera ) const
{
	if (m_trackRenderer->GetTrackRenderer() == TrackRenderType::Debug || m_selectMode)
	{
		for (const auto& node : std::views::values(m_trackManager->GetNodeMap()))
		{
			Engine::Circle::RenderWorldPos(camera, node.nodePosition, .75f, GetColor(Color::TrackRail), 0.f, 6);
			Engine::Circle::RenderWorldPos(camera, node.nodePosition, .75f, GetColor(Color::TrackRail), 0.f, 4);
		}
	}

	if (m_renderConnectedSegments)
	{
		RenderConnectedSegments(camera);
	}

	if (m_selectedSignal != SignalID::Invalid)
	{
		RenderSignal(camera, m_selectedSignal);
	}

	if (m_selectMode)
	{
		if (m_hoveredSegment != TrackSegmentID::Invalid)
		{
			const uint color = GetColor(Color::TrackHover_DEBUG);

			const TrackSegment& t = m_trackManager->GetTrackSegment(m_hoveredSegment);

			const Engine::CurveData data{.lStart = t.nodeA_Position, .lStartDir = t.nodeA_Direction, .lEnd = t.nodeB_Position, .lEndDir = t.nodeB_Direction};

			Engine::CurvedSegment::RenderTrackLinesWorldPos(camera, data, color, .75f);
		}

		if (m_selectedSegment != TrackSegmentID::Invalid)
		{
			const uint color = GetColor(m_selectedSegment == m_hoveredSegment ? Color::TrackHoverSelect_DEBUG : Color::TrackSelect_DEBUG);

			const TrackSegment& t = m_trackManager->GetTrackSegment(m_selectedSegment);

			const Engine::CurveData data{.lStart = t.nodeA_Position, .lStartDir = t.nodeA_Direction, .lEnd = t.nodeB_Position, .lEndDir = t.nodeB_Direction};

			Engine::CurvedSegment::RenderTrackLinesWorldPos(camera, data, color, .75f);
		}

		if (m_hoveredNode != TrackNodeID::Invalid)
		{
			const uint color = GetColor(Color::TrackHover_DEBUG);

			const TrackNode& n = m_trackManager->GetTrackNode(m_hoveredNode);

			Engine::Circle::RenderWorldPos(camera, n.nodePosition, .75f, color, 0.f, 6);
			Engine::Circle::RenderWorldPos(camera, n.nodePosition, .75f, color, 0.f, 4);
		}

		if (m_selectedNode != TrackNodeID::Invalid)
		{
			const uint color = GetColor(m_selectedNode == m_hoveredNode ? Color::TrackHoverSelect_DEBUG : Color::TrackSelect_DEBUG);

			const TrackNode& n = m_trackManager->GetTrackNode(m_selectedNode);

			Engine::Circle::RenderWorldPos(camera, n.nodePosition, .75f, color, 0.f, 6);
			Engine::Circle::RenderWorldPos(camera, n.nodePosition, .75f, color, 0.f, 4);
		}

		if (m_signalManager->IsValidBlock(m_selectedBlock))
		{
			uint seed = static_cast<uint>(m_selectedBlock) * 17;
			uint8_t r = 128 + RandomUInt(seed) % 128;
			uint8_t g = 128 + RandomUInt(seed) % 128;
			uint8_t b = 128 + RandomUInt(seed) % 128;
			uint color = (r << 16) | (g << 8) | b;
			const SignalBlock& block = m_signalManager->GetBlock(m_selectedBlock);
			for (const auto& connectionList : block.connections)
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
					Engine::LineSegment::RenderWorldPos(camera, lStart, lEnd, color, HeightLayer::Debug, 0.5f);
				}
			}
		}
	}
}

void TrackDebugger::UI()
{
	if (!m_visible) return;

	if (Engine::UIManager::BeginDebugWindow("TrackDebugger", &m_visible))
	{
		ImGui::Checkbox("Render connectedSegments", &m_renderConnectedSegments);

		if (ImGui::Checkbox("SelectMode", &m_selectMode))
		{
			if (m_selectMode)
			{
				m_trackRenderer->SetTrackRenderer(TrackRenderType::Debug);
			}
			else
			{
				m_trackRenderer->SetTrackRenderer(TrackRenderType::Default);
			}
		}

		if (ImGui::CollapsingHeader("Segment/Node data", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_selectedSegment != TrackSegmentID::Invalid)
			{
				if (ImGui::TreeNodeEx("Selected Segment", ImGuiTreeNodeFlags_DefaultOpen))
				{
					SegmentInfo(m_selectedSegment);
					ImGui::TreePop();
				}
			}

			if (m_selectedNode != TrackNodeID::Invalid)
			{
				if (ImGui::TreeNodeEx("Selected Node", ImGuiTreeNodeFlags_DefaultOpen))
				{
					NodeInfo(m_selectedNode);
					ImGui::TreePop();
				}
			}

			if (m_hoveredSegment != TrackSegmentID::Invalid && m_hoveredSegment != m_selectedSegment)
			{
				if (ImGui::TreeNodeEx("Hovered Segment", ImGuiTreeNodeFlags_DefaultOpen))
				{
					SegmentInfo(m_hoveredSegment);
					ImGui::TreePop();
				}
			}

			if (m_hoveredNode != TrackNodeID::Invalid && m_hoveredNode != m_selectedNode)
			{
				if (ImGui::TreeNodeEx("Hovered Node", ImGuiTreeNodeFlags_DefaultOpen))
				{
					NodeInfo(m_hoveredNode);
					ImGui::TreePop();
				}
			}
		}
	}
	Engine::UIManager::EndDebugWindow();

	if (Engine::UIManager::BeginDebugWindow("BlockDebugger", &m_visible))
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		m_selectedSignal = SignalID::Invalid;

		const auto& blocks = m_signalManager->GetBlockMap();
		for (const auto& block : std::views::values(blocks))
		{
			bool selected = m_selectedBlock == block.id;
			if (ImGui::Selectable(std::to_string(static_cast<uint>(block.id)).c_str(), &selected))
			{
				if (selected)
				{
					m_selectedBlock = block.id;
				}
			}
			if (selected)
			{
				ImGui::SetCursorPosX(10.f);
				ImGui::Text("Connected Signals: ");
				for (const auto& connections : block.connections)
				{
					ImGui::SetCursorPosX(15.f);

					const ImVec2 cursor = ImGui::GetCursorScreenPos();

					const std::string nodeText = std::to_string(static_cast<int>(connections.first)) + " ";
					const ImVec2 nodeTextSize = ImGui::CalcTextSize(nodeText.c_str());

					ImGui::InvisibleButton(("##signal_" + std::to_string(static_cast<int>(connections.first))).c_str(), nodeTextSize);
					if (ImGui::IsItemHovered())
					{
						m_selectedSignal = connections.first;
						drawList->AddRectFilled(cursor, ImVec2(cursor.x + nodeTextSize.x, cursor.y + nodeTextSize.y), IM_COL32(100, 100, 100, 50));
						ImGui::SetTooltip("Signal");
					}
					drawList->AddText(cursor, IM_COL32(255, 255, 255, 255), nodeText.c_str());

					ImGui::SameLine();
					ImGui::Text(("> "));

					for (auto signal : connections.second)
					{
						ImGui::SameLine();
						const ImVec2 cursor = ImGui::GetCursorScreenPos();

						const std::string nodeText = std::to_string(static_cast<int>(signal)) + " ";
						const ImVec2 nodeTextSize = ImGui::CalcTextSize(nodeText.c_str());

						ImGui::InvisibleButton(("##signal_" + std::to_string(static_cast<int>(signal))).c_str(), nodeTextSize);
						if (ImGui::IsItemHovered())
						{
							m_selectedSignal = signal;
							drawList->AddRectFilled(cursor, ImVec2(cursor.x + nodeTextSize.x, cursor.y + nodeTextSize.y), IM_COL32(100, 100, 100, 50));
							ImGui::SetTooltip("Signal");
						}
						drawList->AddText(cursor, IM_COL32(255, 255, 255, 255), nodeText.c_str());
					}
				}
			}
		}
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

void TrackDebugger::EnableSelectMode( const bool value )
{
	m_selectMode = value;

	if (m_selectMode)
	{
		m_trackRenderer->SetTrackRenderer(TrackRenderType::Debug);
	}
	else
	{
		m_trackRenderer->SetTrackRenderer(TrackRenderType::Default);
	}
}

bool TrackDebugger::GetSelectMode() const
{
	return m_selectMode;
}

void TrackDebugger::SetHoverNode( const TrackNodeID id )
{
	m_hoveredNode = id;
	m_hoveredSegment = TrackSegmentID::Invalid;
	m_hoverSafety = true;
}

void TrackDebugger::SetHoverSegment( const TrackSegmentID id )
{
	m_hoveredSegment = id;
	m_hoveredNode = TrackNodeID::Invalid;
	m_hoverSafety = true;
}

void TrackDebugger::SetSelectNode( const TrackNodeID id )
{
	m_selectedNode = id;
	m_selectedSegment = TrackSegmentID::Invalid;
	m_hoverSafety = true;
}

void TrackDebugger::SetSelectSegment( const TrackSegmentID id )
{
	m_selectedSegment = id;
	m_selectedNode = TrackNodeID::Invalid;
	m_hoverSafety = true;
}

void TrackDebugger::SegmentInfo( const TrackSegmentID segmentID )
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	if (!m_trackManager->DoesSegmentExists(segmentID)) return;
	const TrackSegment& segment = m_trackManager->GetTrackSegment(segmentID);

	ImGui::Text("ID: ");
	ImGui::SameLine();

	const ImVec2 cursor = ImGui::GetCursorScreenPos();

	const std::string nodeText = std::to_string(static_cast<int>(segmentID));
	const ImVec2 nodeTextSize = ImGui::CalcTextSize(nodeText.c_str());

	ImGui::InvisibleButton(("##segment_" + std::to_string(static_cast<int>(segmentID))).c_str(), nodeTextSize);
	if (ImGui::IsItemHovered())
	{
		SetHoverSegment(segmentID);
		drawList->AddRectFilled(cursor, ImVec2(cursor.x + nodeTextSize.x, cursor.y + nodeTextSize.y), IM_COL32(100, 100, 100, 50));
		ImGui::SetTooltip("SegmentID %d", static_cast<int>(segmentID));
	}
	if (ImGui::IsItemClicked())
	{
		SetSelectSegment(segmentID);
	}
	drawList->AddText(cursor, IM_COL32(255, 255, 255, 255), nodeText.c_str());

	ImGui::Text("Segment length: %.2f", segment.distance);

	if (ImGui::TreeNodeEx("Node A", ImGuiTreeNodeFlags_DefaultOpen))
	{
		NodeSegmentInfo(segmentID, true);

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Node B", ImGuiTreeNodeFlags_DefaultOpen))
	{
		NodeSegmentInfo(segmentID, false);

		ImGui::TreePop();
	}
}

void TrackDebugger::NodeSegmentInfo( const TrackSegmentID segmentID, const bool nodeA )
{
	const TrackSegment& segment = m_trackManager->GetTrackSegment(segmentID);

	const TrackNodeID nodeID = nodeA ? segment.nodeA : segment.nodeB;
	const TrackNode& node = m_trackManager->GetTrackNode(nodeID);
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// NODE ID

	ImGui::Text("ID: ");
	ImGui::SameLine();

	ImVec2 cursor = ImGui::GetCursorScreenPos();

	std::string nodeText = std::to_string(static_cast<int>(nodeID));
	ImVec2 nodeTextSize = ImGui::CalcTextSize(nodeText.c_str());

	ImGui::InvisibleButton(("##node_" + std::to_string(static_cast<int>(nodeID))).c_str(), nodeTextSize);
	if (ImGui::IsItemHovered())
	{
		SetHoverNode(nodeID);
		drawList->AddRectFilled(cursor, ImVec2(cursor.x + nodeTextSize.x, cursor.y + nodeTextSize.y), IM_COL32(100, 100, 100, 50));
		ImGui::SetTooltip("NodeID %d", static_cast<int>(nodeID));
	}
	if (ImGui::IsItemClicked())
	{
		SetSelectNode(nodeID);
	}
	drawList->AddText(cursor, IM_COL32(255, 255, 255, 255), nodeText.c_str());

	const float2 nodePos = nodeA ? segment.nodeA_Position : segment.nodeB_Position;
	const float2 nodeDir = nodeA ? segment.nodeA_Direction : segment.nodeB_Direction;

	ImGui::Text("Position: (%.2f, %.2f)", nodePos.x, nodePos.y);
	ImGui::Text("Segment node direction: (%.2f, %.2f)", nodeDir.x, nodeDir.y);

	ImGui::Text("Connections From %d: ", static_cast<int>(segmentID));

	auto& outs = node.validConnections.at(segmentID);

	int lever = -1;
	const auto it = node.connectionLever.find(segmentID);
	if (it != node.connectionLever.end()) lever = it->second;

	cursor = ImGui::GetCursorScreenPos();

	// --- Clickable nodeID ---
	nodeText = std::to_string(static_cast<int>(segmentID));
	nodeTextSize = ImGui::CalcTextSize(nodeText.c_str());

	ImGui::InvisibleButton(("##segment_" + std::to_string(static_cast<int>(segmentID))).c_str(), nodeTextSize);
	if (ImGui::IsItemHovered())
	{
		SetHoverSegment(segmentID);
		drawList->AddRectFilled(cursor, ImVec2(cursor.x + nodeTextSize.x, cursor.y + nodeTextSize.y),IM_COL32(100, 100, 100, 50));
		ImGui::SetTooltip("SegmentID %d", static_cast<int>(segmentID));
	}
	if (ImGui::IsItemClicked())
	{
		SetSelectSegment(segmentID);
	}
	drawList->AddText(cursor, IM_COL32(255, 255, 255, 255), nodeText.c_str());

	ImGui::SameLine(0, 0);
	ImGui::TextUnformatted(" -> ");
	ImGui::SameLine(0, 0);

	if (outs.empty())
	{
		ImGui::Text("No outgoing connections (%d)", lever);
		return;
	}

	for (size_t i = 0; i < outs.size(); ++i)
	{
		if (i > 0)
		{
			ImGui::TextUnformatted(",");
			ImGui::SameLine(0, 0);
		}

		std::string text = std::to_string(static_cast<int>(outs[i]));
		ImVec2 size = ImGui::CalcTextSize(text.c_str());
		ImVec2 pos = ImGui::GetCursorScreenPos();

		std::string id = "##seg_" + std::to_string(static_cast<int>(segmentID)) + "_" + std::to_string(i);
		ImGui::InvisibleButton(id.c_str(), size);

		const bool isHovered = ImGui::IsItemHovered();
		const bool isClicked = ImGui::IsItemClicked();
		const bool isActive = (static_cast<int>(i) == lever);
		bool isSelected = (m_selectedSegment == outs[i]);

		if (isHovered || isActive || isSelected)
		{
			ImU32 bgColor = isSelected ? IM_COL32(100, 200, 255, 100) : isActive ? IM_COL32(255, 255, 0, 60) : IM_COL32(255, 255, 255, 30);
			drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgColor);
		}

		if (isHovered)
		{
			drawList->AddRectFilled(cursor, ImVec2(cursor.x + nodeTextSize.x, cursor.y + nodeTextSize.y), IM_COL32(100, 100, 100, 50));
			ImGui::SetTooltip("SegmentID %d", static_cast<int>(outs[i]));
			SetHoverSegment(outs[i]);
		}

		if (isClicked)
		{
			SetSelectSegment(outs[i]);
		}

		drawList->AddText(pos, IM_COL32(255, 255, 255, 255), text.c_str());

		ImGui::SameLine(0, 0);
	}

	ImGui::SameLine(0, 10);
	ImGui::Text("(%d)", lever);
}

void TrackDebugger::NodeInfo( const TrackNodeID nodeID )
{
	if (!m_trackManager->DoesNodeExists(nodeID)) return;

	const TrackNode& node = m_trackManager->GetTrackNode(nodeID);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImGui::Text("ID: ");
	ImGui::SameLine();

	ImVec2 cursor = ImGui::GetCursorScreenPos();

	std::string nodeText = std::to_string(static_cast<int>(nodeID));
	ImVec2 nodeTextSize = ImGui::CalcTextSize(nodeText.c_str());

	ImGui::InvisibleButton(("##node_" + std::to_string(static_cast<int>(nodeID))).c_str(), nodeTextSize);
	if (ImGui::IsItemHovered())
	{
		SetHoverNode(nodeID);
		drawList->AddRectFilled(cursor, ImVec2(cursor.x + nodeTextSize.x, cursor.y + nodeTextSize.y), IM_COL32(100, 100, 100, 50));
		ImGui::SetTooltip("NodeID %d", static_cast<int>(nodeID));
	}
	if (ImGui::IsItemClicked())
	{
		SetSelectNode(nodeID);
	}
	drawList->AddText(cursor, IM_COL32(255, 255, 255, 255), nodeText.c_str());

	ImGui::Text("Position: (%.2f, %.2f)", node.nodePosition.x, node.nodePosition.y);

	ImGui::Text("Connections: ");

	for (auto& kv : node.validConnections)
	{
		const TrackSegmentID segmentID = kv.first;
		auto& outs = kv.second;

		int lever = -1;
		auto it = node.connectionLever.find(segmentID);
		if (it != node.connectionLever.end())
			lever = it->second;

		cursor = ImGui::GetCursorScreenPos();

		// --- Clickable nodeID ---
		nodeText = std::to_string(static_cast<int>(segmentID));
		nodeTextSize = ImGui::CalcTextSize(nodeText.c_str());
		ImGui::InvisibleButton(("##segment_" + std::to_string(static_cast<int>(segmentID))).c_str(), nodeTextSize);
		if (ImGui::IsItemHovered())
		{
			SetHoverSegment(segmentID);
			drawList->AddRectFilled(cursor, ImVec2(cursor.x + nodeTextSize.x, cursor.y + nodeTextSize.y), IM_COL32(100, 100, 100, 50));
			ImGui::SetTooltip("SegmentID %d", static_cast<int>(segmentID));
		}
		if (ImGui::IsItemClicked())
		{
			SetSelectSegment(segmentID);
		}
		drawList->AddText(cursor, IM_COL32(255, 255, 255, 255), nodeText.c_str());

		ImGui::SameLine(0, 0);
		ImGui::TextUnformatted(" -> ");
		ImGui::SameLine(0, 0);

		if (outs.empty())
		{
			ImGui::Text("No outgoing connections (%d)", lever);
			continue;
		}

		for (size_t i = 0; i < outs.size(); ++i)
		{
			if (i > 0)
			{
				ImGui::TextUnformatted(",");
				ImGui::SameLine(0, 0);
			}

			std::string text = std::to_string(static_cast<int>(outs[i]));
			ImVec2 size = ImGui::CalcTextSize(text.c_str());
			ImVec2 pos = ImGui::GetCursorScreenPos();

			std::string id = "##seg_" + std::to_string(static_cast<int>(segmentID)) + "_" + std::to_string(i);
			ImGui::InvisibleButton(id.c_str(), size);

			const bool isHovered = ImGui::IsItemHovered();
			const bool isClicked = ImGui::IsItemClicked();
			const bool isActive = (static_cast<int>(i) == lever);
			bool isSelected = (m_selectedSegment == outs[i]);

			if (isHovered || isActive || isSelected)
			{
				ImU32 bgColor = isSelected ? IM_COL32(100, 200, 255, 100) : isActive ? IM_COL32(255, 255, 0, 60) : IM_COL32(255, 255, 255, 30);
				drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgColor);
			}

			if (isHovered)
			{
				drawList->AddRectFilled(cursor, ImVec2(cursor.x + nodeTextSize.x, cursor.y + nodeTextSize.y), IM_COL32(100, 100, 100, 50));
				ImGui::SetTooltip("SegmentID %d", static_cast<int>(outs[i]));
				SetHoverSegment(outs[i]);
			}

			if (isClicked)
			{
				SetSelectSegment(outs[i]);
			}

			drawList->AddText(pos, IM_COL32(255, 255, 255, 255), text.c_str());

			ImGui::SameLine(0, 0);
		}

		ImGui::SameLine(0, 10);
		ImGui::Text("(%d)", lever);
	}
}

void TrackDebugger::RenderConnectedSegments( const Engine::Camera& camera ) const
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
		colorIndex %= static_cast<int>(colors.size());

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
			Engine::CurvedSegment::RenderTrackLinesWorldPos(camera, data, colors.at(colorIndex), 0.f);
		}
	}
}

void TrackDebugger::RenderSignal( const Engine::Camera& camera, const SignalID signalID ) const
{
	const Signal& signal = m_signalManager->GetSignal(signalID);
	const TrackSegment& segment = m_trackManager->GetTrackSegment(signal.segment);
	Engine::CurveData curve{segment.nodeA_Position, segment.nodeA_Direction, segment.nodeB_Position, segment.nodeB_Direction};
	float2 pos = Engine::CurvedSegment::GetPositionOnCurvedSegment(signal.distanceOnSegment, curve);

	float2 dir = Engine::CurvedSegment::GetDirectionOnCurvedSegment(signal.distanceOnSegment, curve);
	float2 placeDir = signal.directionTowardsNodeB ? float2(-dir.y, dir.x) : float2(dir.y, -dir.x);
	pos += placeDir * 1.75f;
	Engine::Circle::RenderWorldPos(camera, pos, 0.5f, GetColor(Color::TrackSelect_DEBUG), 0.1f);
}
