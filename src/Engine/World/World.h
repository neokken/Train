#pragma once

// TODO: Either pointerize or include these in any dependency with World.h
#include "Renderables/Grid.h"
#include "Camera/Camera.h"
#include "Data/ParticleSystem.h"
#include "Game/TrainSystem/TrackBuilder.h"
#include "Game/TrainSystem/TrackRenderer.h"
#include "Game/TrainSystem/Debugger/TrainDebugger.h"
#include "Game/TrainSystem/Debugger/TrackDebugger.h"

namespace Engine
{
	class GameObject;
	class InputManager;

	class World
	{
	public:
		World() = default;
		~World();

		// Life cycle functions
		void Init( Surface* renderTarget );
		void Update( float deltaTime );
		void UI( float deltaTime );

		// World management functions
		void AddObject( GameObject* obj );
		void DestroyObject( GameObject* obj );

		const Camera& GetCamera() const;
		Surface* GetRenderTarget() const;

		ParticleSystem& GetParticleSystem() { return m_particles; }
		TrackManager& GetTrackManager() { return m_trackManager; }
		TrainManager& GetTrainManager() { return m_trainManager; }

	private:
		std::vector<GameObject*> m_objects;

		Grid m_grid;
		Camera m_camera{int2(0, 0)};

		TrackManager m_trackManager;
		TrainManager m_trainManager;
		TrackBuilder m_trackBuilder;
		TrackRenderer m_trackRenderer;

		TrackDebugger m_trackDebugger;
		TrainDebugger m_trainDebugger;

		ParticleSystem m_particles = {10000};

		Surface* m_renderTarget = nullptr;
	};
}
