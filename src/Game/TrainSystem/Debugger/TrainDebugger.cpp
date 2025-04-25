#include "precomp.h"
#include "TrainDebugger.h"

#include "Camera/Camera.h"
#include "Game/Trains/Train.h"
#include "Game/Trains/Wagon.h"
#include "Renderables/Circle.h"
#include "UI/UIManager.h"

void TrainDebugger::Init( TrackManager& trackManager, TrainManager& trainManager )
{
	m_trackManager = &trackManager;
	m_trainManager = &trainManager;
}

void TrainDebugger::Update( Engine::Camera& camera, Surface& drawSurface )
{
	const Engine::InputManager& input = Input::get();

	const float2 worldPosMouse = camera.GetWorldPosition(input.GetMousePos());

	auto trainMap = m_trainManager->GetTrainMap();
	for (auto train : trainMap)
	{
		for (auto wagon : train.second->GetWagons())
		{
			if (length(wagon->GetTransform().position - worldPosMouse) < 3.f)
			{
				Engine::Circle::RenderWorldPos(camera, drawSurface, wagon->GetTransform().position, 2.f, 0x3030ff);
				if (input.IsMouseClicked(0))
				{
					m_selectedTrain = train.first;
				}
			}
		}
	}
}

void TrainDebugger::UI()
{
	if (m_selectedTrain != TrainID::Invalid)
	{
		bool open = true;
		Engine::UIManager::BeginGameplayWindow(("Train: " + std::to_string(static_cast<int>(m_selectedTrain))).c_str(), open);
		ImGui::Text("Train");
		Engine::UIManager::EndGameplayWindow();
		if (!open) m_selectedTrain = TrainID::Invalid;
	}
}
