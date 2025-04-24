#include "precomp.h"
#include "Game/Trains/Wagon.h"

#include "Camera/Camera.h"
#include "Renderables/Arrow.h"
#include "Renderables/Circle.h"
#include "Renderables/LineSegment.h"
#include "World/World.h"
#include "Train.h"

Train::Train( std::vector<Wagon*> wagons )
	: GameObject()
	  , m_wagons(wagons)
{
	float currDistance = 0;
	for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
	{
		m_wagons[i]->SetInvincible(true);
		m_wagons[i]->GetFrontWalker().SetCurrentTrackSegment(m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment(), m_wagons[0]->GetFrontWalker().GetDistance());
		currDistance += (m_wagons[i]->GetWagonLength()) + m_wagonSpacing;
		m_wagons[i]->Move(-currDistance);
		TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
		TrackWalker& back = m_wagons[i]->GetFrontWalker();
		for (int j = 0; j < 5; ++j) // Settle in to its position
		{
			float walkerDistance = length(front.GetPosition() - back.GetPosition());
			float diff = walkerDistance - m_wagonSpacing;
			m_wagons[i]->Move(diff);
		}
		//m_wagons[i]->SetInvincible(false);
	}
}

void Train::Update( const float deltaTime )
{
	if (m_wagons.size() == 0) return;
	if (m_moveSpeed > 0)
	{
		m_wagons[0]->Move(deltaTime * m_moveSpeed);
		for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
		{
			TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
			TrackWalker& back = m_wagons[i]->GetFrontWalker();
			float walkerDistance = length(front.GetPosition() - back.GetPosition());
			float diff = walkerDistance - m_wagonSpacing;
			m_wagons[i]->Move(diff);
		}
	}
	else if (m_moveSpeed < 0)
	{
		m_wagons[m_wagons.size() - 1]->Move(deltaTime * m_moveSpeed);
		for (int i = static_cast<int>(m_wagons.size()) - 2; i >= 0; --i)
		{
			TrackWalker& front = m_wagons[i + 1]->GetFrontWalker();
			TrackWalker& back = m_wagons[i]->GetBackWalker();
			float walkerDistance = length(front.GetPosition() - back.GetPosition());
			m_wagons[i]->Move(m_wagonSpacing - walkerDistance);
		}
	}
}

void Train::Render( const Engine::Camera& camera, Surface& target )
{
	for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
	{
		TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
		TrackWalker& back = m_wagons[i]->GetFrontWalker();
		Engine::LineSegment::RenderWorldPos(camera, front.GetPosition(), back.GetPosition(), m_wireColor);
	}
}

void Train::ImGuiDebugViewer()
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
	ImGui::DragFloat("MoveSpeed", &m_moveSpeed, .1f);
}
