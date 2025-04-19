#include "precomp.h"
#include "Game/Trains/Wagon.h"

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
	if (!m_locked)
	{
		//Physics calculations
		m_velocity += m_acceleration * deltaTime; // Technically frame dependent but good enough

		float drag = m_airDragCoefficient * WORLD_AIR_DENSITY * (m_velocity * m_velocity) / 2;
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
		//Movement
		float tensionForce{0.f};
		float2 worldVelocityFront;
		float2 worldVelocityBack;
		if (m_velocity > 0)
		{
			float2 oldFrontPos = m_frontWalker.GetPosition();
			m_frontWalker.Move(m_velocity * deltaTime);
			worldVelocityFront = (oldFrontPos - m_frontWalker.GetPosition());
			float frontPosChange = length(worldVelocityFront);
			worldVelocityFront = worldVelocityFront / max(frontPosChange, 0.0001f);

			float walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());

			float2 oldBackPos = m_backWalker.GetPosition();
			m_backWalker.Move(walkerDistance - m_wagonLength);
			worldVelocityBack = (oldBackPos - m_backWalker.GetPosition());
			float backPosChange = length(worldVelocityBack);
			worldVelocityBack = worldVelocityBack / max(backPosChange, 0.0001f);

			tensionForce = abs(frontPosChange - backPosChange);
		}
		else if (m_velocity < 0)
		{
			float2 oldBackPos = m_backWalker.GetPosition();
			m_backWalker.Move(m_velocity * deltaTime);
			worldVelocityBack = (oldBackPos - m_backWalker.GetPosition());
			float backPosChange = length(worldVelocityBack);
			worldVelocityBack = worldVelocityBack / max(backPosChange, 0.0001f);

			float walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());

			float2 oldFrontPos = m_frontWalker.GetPosition();
			m_frontWalker.Move(m_wagonLength - walkerDistance);
			worldVelocityFront = (oldFrontPos - m_frontWalker.GetPosition());
			float frontPosChange = length(worldVelocityFront);
			worldVelocityFront = worldVelocityFront / max(frontPosChange, 0.0001f);

			tensionForce = abs(frontPosChange - backPosChange);
		}
		if (tensionForce > m_maxTensionForce) Derail();
		float velocityChange = 1.f - abs(dot(m_lastWorldVelocityFront, worldVelocityFront));
		velocityChange += 1.f - abs(dot(m_lastWorldVelocityBack, worldVelocityBack));
		velocityChange *= 25.f;
		m_lastWorldVelocityBack = worldVelocityBack;
		m_lastWorldVelocityFront = worldVelocityFront;
		float trackDrag = m_trackDragCoefficient * WORLD_TRACK_ROUGHNESS * (velocityChange) * (m_velocity * m_velocity) / 2;
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
		printf("Drag:\n Air: %f\n Track: %f\n", drag, trackDrag);
	}
	m_transform.position = m_frontWalker.GetPosition();
}

void Wagon::Move( const float distance )
{
	if (distance > 0)
	{
		float2 oldFrontPos = m_frontWalker.GetPosition();
		m_frontWalker.Move(distance);
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
		m_backWalker.Move(distance);
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
	if (m_invincible) return;
	printf("Train Derailed!!!\n");
	float2 dir = m_frontWalker.GetPosition() - m_backWalker.GetPosition();
	float2 pos = m_backWalker.GetPosition() + dir / 2;

	m_world->GetParticleSystem().SpawnParticles(pos, 0, int2(5000, 8000), float2(1.f, 30.f), float2(0.1f, .5f), float2(0.1f, 1.f), 0xffbb20, 0xaa3000);

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

	ImGui::DragFloat("Velocity", &m_velocity, .1f);
	ImGui::DragFloat("Acceleration", &m_acceleration, .1f);
}
