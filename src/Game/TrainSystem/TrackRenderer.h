#pragma once

namespace Engine
{
	class Camera;
}

class TrackManager;
struct TrackSegment;

enum class TrackRenderType
{
	Default,
	RailsOnly,
	Debug,
};

class TrackRenderer
{
public:
	TrackRenderer() = default;

	void Init( TrackManager* trackManager );

	void Render( const Engine::Camera& camera, Surface& targetSurface ) const;

	void SetTrackRenderer( TrackRenderType type );

	static void RenderTrackSegment( const Engine::Camera& camera, Surface& targetSurface, const TrackSegment& segment, TrackRenderType type, Color trackColor = Color::TrackRail, Color SpokeColor = Color::TrackSpokes );

private:
	TrackManager* m_trackManager{nullptr};

	TrackRenderType m_currentTrackRenderer{TrackRenderType::Default};
};
