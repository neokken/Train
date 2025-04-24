#include "precomp.h"
#include "Grid.h"

#include "Camera/Camera.h"
#include "LineSegment.h"

void Engine::Grid::Render( const Camera& camera )
{
	const float cameraZoomLevel = camera.GetZoomLevel();

	const float2 topLeftWorldPos = camera.GetTopLeft();
	const float2 bottomRightWorldPos = camera.GetBottomRight();

	for (const auto& gridData : m_gridLineData)
	{
		if (cameraZoomLevel < gridData.minZoomLevel || cameraZoomLevel > gridData.maxZoomLevel)
		{
			continue;
		}

		const int startX = static_cast<int>(floorf(topLeftWorldPos.x / static_cast<float>(gridData.pixelDistance)));
		const int endX = static_cast<int>(floorf(bottomRightWorldPos.x / static_cast<float>(gridData.pixelDistance)));

		const int startY = static_cast<int>(floorf(topLeftWorldPos.y / static_cast<float>(gridData.pixelDistance)));
		const int endY = static_cast<int>(floorf(bottomRightWorldPos.y / static_cast<float>(gridData.pixelDistance)));

		for (int i = startX; i <= endX; i++)
		{
			const int x = i * gridData.pixelDistance;

			LineSegment::RenderWorldPos(camera, float2(static_cast<float>(x), topLeftWorldPos.y), float2(static_cast<float>(x), bottomRightWorldPos.y), gridData.lineColor);
		}

		for (int i = startY; i <= endY; i++)
		{
			const int y = i * gridData.pixelDistance;

			LineSegment::RenderWorldPos(camera, float2(topLeftWorldPos.x, static_cast<float>(y)), float2(bottomRightWorldPos.x, static_cast<float>(y)), gridData.lineColor);
		}
	}
}

void Engine::Grid::AddGrid( uint color, int pixelDistance, float minZoomLevel, float maxZoomLevel )
{
	m_gridLineData.emplace_back(color, pixelDistance, minZoomLevel, maxZoomLevel);
}
