#include "precomp.h"
#include "Wagon.h"

#include "Camera/Camera.h"
#include "Renderables/Arrow.h"
#include "Renderables/Circle.h"
#include "Renderables/LineSegment.h"
#include "World/World.h"

Wagon::Wagon( const TrackWalker& trainWalker )
	: GameObject()
	  , m_frontWalker(trainWalker)
	  , m_backWalker(trainWalker)
{
	//Set up wagon distance
	m_frontWalker.Move(m_wagonLength / 2.f);
	m_backWalker.Move(-m_wagonLength / 2.f);

	for (int i = 0; i < 4; ++i) // Do this a couple of times to make sure the train is settled and doesn't derail on spawn
	{
		float walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());
		m_frontWalker.Move(m_wagonLength - walkerDistance);

		walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());
		m_backWalker.Move(walkerDistance - m_wagonLength);
	}
}

void Wagon::Update( const float deltaTime )
{
	if (m_moveSpeed > 0)
	{
		float2 oldFrontPos = m_frontWalker.GetPosition();
		m_frontWalker.Move(m_moveSpeed * deltaTime);
		float frontPosChange = length(oldFrontPos - m_frontWalker.GetPosition());

		float walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());

		float2 oldBackPos = m_backWalker.GetPosition();
		m_backWalker.Move(walkerDistance - m_wagonLength);
		float backPosChange = length(oldBackPos - m_backWalker.GetPosition());

		float tensionForce = abs(frontPosChange - backPosChange);
		if (tensionForce > m_maxTensionForce) Derail();
	}
	else
	{
		float2 oldBackPos = m_backWalker.GetPosition();
		m_backWalker.Move(m_moveSpeed * deltaTime);
		float backPosChange = length(oldBackPos - m_backWalker.GetPosition());

		float walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());

		float2 oldFrontPos = m_frontWalker.GetPosition();
		m_frontWalker.Move(m_wagonLength - walkerDistance);
		float frontPosChange = length(oldFrontPos - m_frontWalker.GetPosition());

		float tensionForce = abs(frontPosChange - backPosChange);
		if (tensionForce > m_maxTensionForce) Derail();
	}

	m_transform.position = m_frontWalker.GetPosition();
}

void Wagon::Render( const Engine::Camera& camera, Surface& target )
{
	Engine::LineSegment::RenderWorldPos(camera, target, m_frontWalker.GetPosition(), m_backWalker.GetPosition(), 0x0000ff);

	//Front bogey
	float2 scale = float2(1.7f, 2.f);
	target.Rectangle(camera.GetCameraPosition(m_frontWalker.GetPosition()), m_frontWalker.GetDirection(), camera.GetZoomLevel() * scale, m_bogeyColor);
	Engine::Arrow::RenderWorldPos(camera, target, m_frontWalker.GetPosition(), m_frontWalker.GetDirection(), 0.5f, 0xff0000);
	//Back bogey
	target.Rectangle(camera.GetCameraPosition(m_backWalker.GetPosition()), m_backWalker.GetDirection(), camera.GetZoomLevel() * scale, m_bogeyColor);
	Engine::Arrow::RenderWorldPos(camera, target, m_backWalker.GetPosition(), m_backWalker.GetDirection(), 0.5f, 0xff0000);

	//Wagon
	float2 dir = m_frontWalker.GetPosition() - m_backWalker.GetPosition();
	float dirLength = length(dir);
	scale = float2(1.7f, m_wagonLength + 2.15f);
	float2 pos = m_backWalker.GetPosition() + dir / 2;
	dir /= max(dirLength, 0.0001f);
	target.Rectangle(camera.GetCameraPosition(pos), dir, camera.GetZoomLevel() * scale, m_wagonColor);
	target.Rectangle(camera.GetCameraPosition(m_frontWalker.GetPosition()), dir, camera.GetZoomLevel() * float2(1.6f, 2.7f), m_wagonColor);
	target.Rectangle(camera.GetCameraPosition(m_frontWalker.GetPosition()), dir, camera.GetZoomLevel() * float2(1.4f, 2.8f), m_wagonColor);
	target.Rectangle(camera.GetCameraPosition(m_frontWalker.GetPosition()), dir, camera.GetZoomLevel() * float2(1.2f, 3.0f), m_wagonColor);
	target.Rectangle(camera.GetCameraPosition(m_frontWalker.GetPosition()), dir, camera.GetZoomLevel() * float2(.9f, 3.2f), m_wagonColor);
}

void Wagon::Derail()
{
	printf("Train Derailed!!!");
	Destroy();
}

void Wagon::ImGuiDebugViewer()
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
		m_frontWalker.ImGuiDebugViewer();
	}

	ImGui::DragFloat("MoveSpeed", &m_moveSpeed, .1f);
}
