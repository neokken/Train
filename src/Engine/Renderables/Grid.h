#pragma once
#include "Renderable.h"

struct GridLineData
{
	GridLineData(const uint color, const int distance, const float minZoom, const float maxZoom)
		: lineColor(color)
		, pixelDistance(distance)
		, minZoomLevel(minZoom)
		, maxZoomLevel(maxZoom) {}

	uint lineColor;
	int pixelDistance;

	float minZoomLevel;
	float maxZoomLevel;
};

class Grid final : public Renderable
{
public:
	Grid() = default;

	void Render(const Camera& camera, Surface& drawSurface) override;

	void AddGrid(uint color, int pixelDistance, float minZoomLevel=0.f, float maxZoomLevel=1e10f);

private:
	std::vector<GridLineData> gridLineData;

};

