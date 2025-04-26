#include "precomp.h"
#include "Game/Trains/Wagon.h"

#include "Camera/Camera.h"
#include "Helpers/Renderer.h"
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
	if (!m_locked) // This is currently a bit outdated however I feel like you always want a wagon to be moved as part of a train so this shouldn't really be used
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
		float backDirChange, frontDirChange;
		float backPosChange, frontPosChange;
		if (m_velocity > 0)
		{
			float2 oldFrontDir = m_frontWalker.GetDirection();
			float2 oldFrontPos = m_frontWalker.GetPosition();
			m_frontWalker.Move(m_velocity * deltaTime);
			frontDirChange = 1.f - dot(oldFrontDir, m_frontWalker.GetDirection());
			frontPosChange = length(oldFrontPos - m_frontWalker.GetPosition());

			float walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());

			float2 oldBackDir = m_frontWalker.GetDirection();
			float2 oldBackPos = m_backWalker.GetPosition();
			m_backWalker.Move(walkerDistance - m_wagonLength);
			backPosChange = length(oldBackPos - m_backWalker.GetPosition());
			backDirChange = 1.f - dot(oldBackDir, m_frontWalker.GetDirection());

			tensionForce = abs(frontPosChange - backPosChange);
			if (tensionForce > m_maxTensionForce * deltaTime) DebugBreak(), Derail();
		}
		else
		{
			float2 oldBackDir = m_frontWalker.GetDirection();

			float2 oldBackPos = m_backWalker.GetPosition();
			m_backWalker.Move(m_velocity * deltaTime);
			backDirChange = 1.f - dot(oldBackDir, m_frontWalker.GetDirection());
			backPosChange = length(oldBackPos - m_backWalker.GetPosition());

			float walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());

			float2 oldFrontDir = m_frontWalker.GetDirection();
			float2 oldFrontPos = m_frontWalker.GetPosition();
			m_frontWalker.Move(m_wagonLength - walkerDistance);
			frontDirChange = 1.f - dot(oldFrontDir, m_frontWalker.GetDirection());
			frontPosChange = length(oldFrontPos - m_frontWalker.GetPosition());

			tensionForce = abs(frontPosChange - backPosChange);
			if (tensionForce > m_maxTensionForce * deltaTime) DebugBreak(), Derail();
		}
		if (tensionForce > m_maxTensionForce) Derail();
		float velocityChange = frontDirChange + backDirChange;
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
	}
	m_transform.position = m_frontWalker.GetPosition();
}

WagonMovementInfo Wagon::Move( const float distance, float deltaTime )
{
	if (deltaTime == 0.f) deltaTime = 1.f;

	float backDirChange, frontDirChange;
	float backPosChange, frontPosChange;
	if (distance > 0)
	{
		float2 oldFrontDir = m_frontWalker.GetDirection();
		float2 oldFrontPos = m_frontWalker.GetPosition();
		m_frontWalker.Move(distance);

		frontDirChange = 1.f - dot(oldFrontDir, m_frontWalker.GetDirection());
			frontPosChange = length(oldFrontPos - m_frontWalker.GetPosition());

		float walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());

		float2 oldBackDir = m_backWalker.GetDirection();
		float2 oldBackPos = m_backWalker.GetPosition();
		m_backWalker.Move(walkerDistance - m_wagonLength);
		backPosChange = length(oldBackPos - m_backWalker.GetPosition());
		backDirChange = 1.f - dot(oldBackDir, m_backWalker.GetDirection());

		float tensionForce = abs(frontPosChange - backPosChange);
		if (tensionForce > m_maxTensionForce * deltaTime) Derail();
	}
	else
	{
		float2 oldBackDir = m_backWalker.GetDirection();

		float2 oldBackPos = m_backWalker.GetPosition();
		m_backWalker.Move(distance);
		backDirChange = 1.f - dot(oldBackDir, m_backWalker.GetDirection());
		backPosChange = length(oldBackPos - m_backWalker.GetPosition());

		float walkerDistance = length(m_frontWalker.GetPosition() - m_backWalker.GetPosition());

		float2 oldFrontDir = m_frontWalker.GetDirection();
		float2 oldFrontPos = m_frontWalker.GetPosition();
		m_frontWalker.Move(m_wagonLength - walkerDistance);

		frontDirChange = 1.f - dot(oldFrontDir, m_frontWalker.GetDirection());
			frontPosChange = length(oldFrontPos - m_frontWalker.GetPosition());

		float tensionForce = abs(frontPosChange - backPosChange);
		if (tensionForce > m_maxTensionForce * deltaTime) Derail();
	}
	float velocityChange = frontDirChange + backDirChange;
	velocityChange *= m_trackDragCoefficient * distance * m_mass;
	//if (velocityChange > 150.f) DebugBreak();

	if (velocityChange > 0.f)
	{
		float amount = velocityChange * 5;
		float speed = min(velocityChange * 10, 10.f);
		if (amount > 1.f) m_world->GetParticleSystem().SpawnParticles(m_frontWalker.GetPosition(), 0.5f, static_cast<int>(amount), float2(speed, speed), float2(0.1f, 0.4f), 0.1f, 0xffffff, 0xffee00);
	}
	if (velocityChange > 0.f)
	{
		float amount = velocityChange * 5;
		float speed = min(velocityChange * 10, 10.f);
		if (amount > 1.f) m_world->GetParticleSystem().SpawnParticles(m_backWalker.GetPosition(), 0.5f, static_cast<int>(amount), float2(speed, speed), float2(0.1f, 0.4f), 0.1f, 0xffffff, 0xffee00);
	}
	m_transform.position = m_frontWalker.GetPosition();
	return {velocityChange};
}

void Wagon::Render( const Engine::Camera& camera )
{
	Engine::LineSegment::RenderWorldPos(camera, m_frontWalker.GetPosition(), m_backWalker.GetPosition(), 0x0000ff);

	Engine::Renderer& renderer = Engine::Renderer::GetRenderer();
	//Front bogey
	float2 scale = float2(0.85f, 1.f);
	renderer.DrawRectangle(float3(camera.GetCameraPosition(m_frontWalker.GetPosition()), HeightLayer::Trains - 1), m_frontWalker.GetDirection(), camera.GetZoomLevel() * scale, Engine::RGB8ToRGB32(m_bogeyColor));
	Engine::Arrow::RenderWorldPos(camera, m_frontWalker.GetPosition(), m_frontWalker.GetDirection(), 0.5f, 0xff0000);
	//Back bogey
	renderer.DrawRectangle(float3(camera.GetCameraPosition(m_backWalker.GetPosition()), HeightLayer::Trains - 1), m_backWalker.GetDirection(), camera.GetZoomLevel() * scale, Engine::RGB8ToRGB32(m_bogeyColor));
	Engine::Arrow::RenderWorldPos(camera, m_backWalker.GetPosition(), m_backWalker.GetDirection(), 0.5f, 0xff0000);

	//Wagon
	float2 dir = m_frontWalker.GetPosition() - m_backWalker.GetPosition();
	float dirLength = length(dir);
	scale = float2(1.7f, (m_wagonLength + 2.15f)) * 0.5f;
	float2 pos = m_backWalker.GetPosition() + dir / 2;
	dir /= max(dirLength, 0.0001f);
	renderer.DrawRectangle(float3(camera.GetCameraPosition(pos), HeightLayer::Trains), dir, camera.GetZoomLevel() * scale, Engine::RGB8ToRGB32(m_wagonColor));
	renderer.DrawRectangle(float3(camera.GetCameraPosition(m_frontWalker.GetPosition()), HeightLayer::Trains), dir, camera.GetZoomLevel() * float2(0.8f, 1.35f), Engine::RGB8ToRGB32(m_wagonColor));
	renderer.DrawRectangle(float3(camera.GetCameraPosition(m_frontWalker.GetPosition()), HeightLayer::Trains), dir, camera.GetZoomLevel() * float2(0.7f, 1.4f), Engine::RGB8ToRGB32(m_wagonColor));
	renderer.DrawRectangle(float3(camera.GetCameraPosition(m_frontWalker.GetPosition()), HeightLayer::Trains), dir, camera.GetZoomLevel() * float2(0.6f, 1.5f), Engine::RGB8ToRGB32(m_wagonColor));
	renderer.DrawRectangle(float3(camera.GetCameraPosition(m_frontWalker.GetPosition()), HeightLayer::Trains), dir, camera.GetZoomLevel() * float2(0.45f, 1.6f), Engine::RGB8ToRGB32(m_wagonColor));
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
