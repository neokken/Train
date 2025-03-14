#include "precomp.h"
#include "Building.h"
#include "Renderables/LineSegment.h"

#include "World/World.h"

Game::Building::Building( Engine::Transform transform ) : GameObject::GameObject( transform )
{
}

void Game::Building::Update( float deltaTime )
{
    // A ChatGPT example to draw a factory
    m_time += deltaTime;
    auto& camera = m_world->GetCamera();
    auto& target = *m_world->GetRenderTarget();

    float2 base = m_transform.position;

    // Factory base (200x100 rectangle)
    Engine::LineSegment::RenderWorldPos( camera, target, base, base + float2{ 200.0f, 0.0f }, 0x00ff00 ); // Bottom
    Engine::LineSegment::RenderWorldPos( camera, target, base + float2{ 200.0f, 0.0f }, base + float2{ 200.0f, -100.0f }, 0x00ff00 ); // Right
    Engine::LineSegment::RenderWorldPos( camera, target, base + float2{ 200.0f, -100.0f }, base + float2{ 0.0f, -100.0f }, 0x00ff00 ); // Top
    Engine::LineSegment::RenderWorldPos( camera, target, base + float2{ 0.0f, -100.0f }, base, 0x00ff00 ); // Left

    // Chimney (on top right)
    float2 chimneyBase = base + float2{ 160.0f, -100.0f };
    Engine::LineSegment::RenderWorldPos( camera, target, chimneyBase, chimneyBase + float2{ 20.0f, 0.0f }, 0xff0000 ); // Bottom
    Engine::LineSegment::RenderWorldPos( camera, target, chimneyBase + float2{ 20.0f, 0.0f }, chimneyBase + float2{ 20.0f, -50.0f }, 0xff0000 ); // Right
    Engine::LineSegment::RenderWorldPos( camera, target, chimneyBase + float2{ 20.0f, -50.0f }, chimneyBase + float2{ 0.0f, -50.0f }, 0xff0000 ); // Top
    Engine::LineSegment::RenderWorldPos( camera, target, chimneyBase + float2{ 0.0f, -50.0f }, chimneyBase, 0xff0000 ); // Left

    // Roof Detail (triangular peak)
    float2 roofTop = base + float2{ 100.0f, -120.0f };
    Engine::LineSegment::RenderWorldPos( camera, target, base + float2{ 0.0f, -100.0f }, roofTop, 0x00ffff );
    Engine::LineSegment::RenderWorldPos( camera, target, roofTop, base + float2{ 200.0f, -100.0f }, 0x00ffff );

    // Door
    float2 doorBase = base + float2{ 80.0f, 0.0f };
    Engine::LineSegment::RenderWorldPos( camera, target, doorBase, doorBase + float2{ 40.0f, 0.0f }, 0xffffff );
    Engine::LineSegment::RenderWorldPos( camera, target, doorBase + float2{ 40.0f, 0.0f }, doorBase + float2{ 40.0f, -40.0f }, 0xffffff );
    Engine::LineSegment::RenderWorldPos( camera, target, doorBase + float2{ 40.0f, -40.0f }, doorBase + float2{ 0.0f, -40.0f }, 0xffffff );
    Engine::LineSegment::RenderWorldPos( camera, target, doorBase + float2{ 0.0f, -40.0f }, doorBase, 0xffffff );

    // Windows (just small squares)
    for (int i = 0; i < 3; i++)
    {
        float2 windowBase = base + float2{ 20.0f + i * 60.0f, -60.0f };
        Engine::LineSegment::RenderWorldPos( camera, target, windowBase, windowBase + float2{ 20.0f, 0.0f }, 0xffff00 );
        Engine::LineSegment::RenderWorldPos( camera, target, windowBase + float2{ 20.0f, 0.0f }, windowBase + float2{ 20.0f, -20.0f }, 0xffff00 );
        Engine::LineSegment::RenderWorldPos( camera, target, windowBase + float2{ 20.0f, -20.0f }, windowBase + float2{ 0.0f, -20.0f }, 0xffff00 );
        Engine::LineSegment::RenderWorldPos( camera, target, windowBase + float2{ 0.0f, -20.0f }, windowBase, 0xffff00 );
    }

    // Draw "cloud" smoke puffs from chimney
    float2 chimneyTop = chimneyBase + float2{ 10.0f, -50.0f }; // Center of chimney opening
    float cloudRadius = 10.0f;
    int cloudSegments = 12; // The more segments, the smoother the "circle"
    int numClouds = 8;

    for (int j = 0; j < numClouds; j++)
    {
        float timeOffset = j * 0.3f;  // Each cloud moves with a slight delay
        float phase = m_time + timeOffset; // Staggered timing for each puff

        float driftX = sin( phase ) * 5.0f + j * 1.5f;  // Horizontal wobble + slow drift
        float driftY = cos( phase * 0.8f ) * 3.0f;      // Vertical oscillation

        // Clouds start small and grow as they rise
        float cloudSize = cloudRadius * (0.8f + 0.1f * j);

        // Apply drift
        float2 cloudCenter = chimneyTop + float2{ driftX, -20.0f * j + driftY };

        for (int i = 0; i < cloudSegments; i++)
        {
            float angle1 = (2.0f * PI * i) / cloudSegments;
            float angle2 = (2.0f * PI * (i + 1)) / cloudSegments;

            float2 p1 = cloudCenter + float2{ cloudSize * cos( angle1 ), cloudSize * sin( angle1 ) };
            float2 p2 = cloudCenter + float2{ cloudSize * cos( angle2 ), cloudSize * sin( angle2 ) };

            Engine::LineSegment::RenderWorldPos( camera, target, p1, p2, 0xaaaaaa ); // Light gray smoke
        }
    }


}

