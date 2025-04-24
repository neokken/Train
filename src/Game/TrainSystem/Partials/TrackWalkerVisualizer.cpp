#include "precomp.h"
#include "TrackWalkerVisualizer.h"

#include "Camera/Camera.h"
#include "Helpers/Renderer.h"
#include "Renderables/Arrow.h"
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

void TrackWalkerVisualizer::Render( const Engine::Camera& camera )
{
	const float2 scale = m_transform.scale * 2.5f * float2(0.5f, 1.f) * 0.5f;
	Engine::Renderer::GetRenderer().DrawRectangle(float3(camera.GetCameraPosition(m_transform.position), HeightLayer::Debug), m_walker.GetDirection(), camera.GetZoomLevel() * scale, Engine::RGB8ToRGB32(m_color));
	Engine::Arrow::RenderWorldPos(camera, m_transform.position, m_walker.GetDirection(), 2.f, 0xff0000);
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
