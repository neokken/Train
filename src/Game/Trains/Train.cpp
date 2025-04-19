#include "precomp.h"
#include "Game/Trains/Wagon.h"

#include "Camera/Camera.h"
#include "Renderables/Arrow.h"
#include "Renderables/Circle.h"
#include "Renderables/LineSegment.h"
#include "World/World.h"
#include "Train.h"

Train::Train( const std::vector<Wagon*>& wagons )
	: GameObject()
{
	for (auto wagon : wagons)
	{
		m_wagons.emplace_back(wagon);
	}

	float currDistance = 0;
	m_wagons[0]->SetLocked(true);

	for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
	{
		m_wagons[i]->SetLocked(true);
		m_wagons[i]->SetInvincible(true);
		m_wagons[i]->GetFrontWalker().SetCurrentTrackSegment(m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment(), m_wagons[0]->GetFrontWalker().GetDistance());
		currDistance += (m_wagons[i]->GetWagonLength()) + m_wagonSpacing;
		m_wagons[i]->Move(-currDistance, 0);
		//TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
		//TrackWalker& back = m_wagons[i]->GetFrontWalker();
		//for (int j = 0; j < 5; ++j) // Settle in to its position
		//{
		//	float walkerDistance = length(front.GetPosition() - back.GetPosition());
		//	float diff = walkerDistance - m_wagonSpacing;
		//	m_wagons[i]->Move(diff, 0);
		//}
		m_wagons[i]->SetInvincible(false);
	}
}

void Train::Update( const float deltaTime )
{
	return;
	if (m_wagons.empty()) return;

	//Physics calculations
	m_velocity += m_acceleration * deltaTime; // Technically frame dependent but good enough

	float airDragCoefficient = m_velocity > 0 ? m_wagons[0]->GetAirDragCoefficient() : m_wagons[m_wagons.size() - 1]->GetAirDragCoefficient();
	float drag = airDragCoefficient * WORLD_AIR_DENSITY * (m_velocity * m_velocity) / 2;
	drag = m_velocity > 0 ? drag : -drag;
	float newVelocity = m_velocity - drag * deltaTime; // Technically frame dependent but good enough
	if ((m_velocity > 0 && newVelocity < 0) || (m_velocity < 0 && newVelocity > 0)) // Fix overcorrecting
	{
		m_velocity = 0.0f;
	}
	else
	{
		m_velocity = newVelocity;
	}

	float velocityChange = 0.f;
	if (m_velocity > 0)
	{
		m_wagons[0]->Move(deltaTime * m_velocity, deltaTime);
		for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
		{
			TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
			TrackWalker& back = m_wagons[i]->GetFrontWalker();

			float walkerDistance = length(front.GetPosition() - back.GetPosition());
			float diff = walkerDistance - m_wagonSpacing;
			WagonMovementInfo moveInfo = m_wagons[i]->Move(diff, deltaTime);
			velocityChange += moveInfo.velocityChangeAmount;
		}
	}
	else if (m_velocity < 0)
	{
		m_wagons[m_wagons.size() - 1]->Move(deltaTime * m_velocity, deltaTime);
		for (int i = static_cast<int>(m_wagons.size()) - 2; i >= 0; --i)
		{
			TrackWalker& front = m_wagons[i + 1]->GetFrontWalker();
			TrackWalker& back = m_wagons[i]->GetBackWalker();
			float walkerDistance = length(front.GetPosition() - back.GetPosition());
			float diff = m_wagonSpacing - walkerDistance;
			WagonMovementInfo moveInfo = m_wagons[i]->Move(diff, deltaTime);
			velocityChange += moveInfo.velocityChangeAmount;
		}
	}

	float trackDrag = WORLD_TRACK_ROUGHNESS * (velocityChange) * (m_velocity * m_velocity) / 2;
	trackDrag = m_velocity > 0 ? trackDrag : -trackDrag;
	newVelocity = m_velocity - trackDrag * deltaTime; // Technically frame dependent but good enough
	if ((m_velocity > 0 && newVelocity < 0) || (m_velocity < 0 && newVelocity > 0)) // Fix overcorrecting
	{
		m_velocity = 0.0f;
	}
	else
	{
		m_velocity = newVelocity;
	}
}

void Train::Render( const Engine::Camera& camera, Surface& target )
{
	return;
	for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
	{
		TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
		TrackWalker& back = m_wagons[i]->GetFrontWalker();
		Engine::LineSegment::RenderWorldPos(camera, target, front.GetPosition(), back.GetPosition(), m_wireColor);
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

	ImGui::DragFloat("Velocity", &m_velocity, .1f);
	ImGui::DragFloat("Acceleration", &m_acceleration, .1f);
}
