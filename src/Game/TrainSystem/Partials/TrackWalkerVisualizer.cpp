#include "precomp.h"
#include "TrackWalkerVisualizer.h"

#include "Renderables/LineSegment.h"

TrackWalkerVisualizer::TrackWalkerVisualizer( const TrackWalker& trainWalker )
	: GameObject()
	  , m_walker(trainWalker)
{
}

void TrackWalkerVisualizer::Update( const float deltaTime )
{
	m_walker.Move(m_moveSpeed * deltaTime);
	m_transform.position = m_walker.GetPosition();
}

void TrackWalkerVisualizer::Render( const Engine::Camera& camera, Surface& target )
{
	const float2 halfScale = m_transform.scale * 0.5f * 10.f; //10 is default scale for TrackWalkerVisualizer

	// Define the four corners of the box
	const float2 topLeft = m_transform.position - halfScale;
	const float2 topRight = m_transform.position + float2(halfScale.x, -halfScale.y);
	const float2 bottomLeft = m_transform.position + float2(-halfScale.x, halfScale.y);
	const float2 bottomRight = m_transform.position + halfScale;

	// Draw the four edges
	Engine::LineSegment::RenderWorldPos(camera, target, topLeft, topRight, m_color); // Top edge
	Engine::LineSegment::RenderWorldPos(camera, target, topRight, bottomRight, m_color); // Right edge
	Engine::LineSegment::RenderWorldPos(camera, target, bottomRight, bottomLeft, m_color); // Bottom edge
	Engine::LineSegment::RenderWorldPos(camera, target, bottomLeft, topLeft, m_color); // Left edge
}

void TrackWalkerVisualizer::ImGuiDebugViewer()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::TreeNode("Info"))
		{
			ImGui::Text("These values are not changeable since they are reset/recalculated every frame");
			ImGui::Text("if you wish to set the position. Look into getting access to the trackwalker");
			ImGui::Text("and setting the currentTrackID and distance..");
			ImGui::TreePop();
		}

		ImGui::Text("Position: %.2f %.2f", m_transform.position.x, m_transform.position.y);
		ImGui::Text("Scale: %.2f %.2f", m_transform.scale.x, m_transform.scale.y);
	}

	if (ImGui::CollapsingHeader("TrackWalker"))
	{
		m_walker.ImGuiDebugViewer();
	}

	ImGui::DragFloat("MoveSpeed", &m_moveSpeed, .1f);
}
