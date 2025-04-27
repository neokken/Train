#pragma once

struct Signal;
class SignalManager;

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

inline const char* to_string( TrackRenderType e )
{
	switch (e)
	{
	case TrackRenderType::Default: return "Default";
	case TrackRenderType::RailsOnly: return "RailsOnly";
	case TrackRenderType::Debug: return "Debug";
	default: return "unknown";
	}
}

class TrackRenderer
{
public:
	TrackRenderer() = default;

	void Init( TrackManager& trackManager, SignalManager& signalManager );

	void Render( const Engine::Camera& camera ) const;

	void SetTrackRenderer( TrackRenderType type );
	[[nodiscard]] TrackRenderType GetTrackRenderer() const;

	static void RenderTrackSegment( const Engine::Camera& camera, const TrackSegment& segment, TrackRenderType type, Color trackColor = Color::TrackRail, Color SpokeColor = Color::TrackSpokes );
	void RenderTrackSignal( const Engine::Camera& camera, const Signal& signal ) const;

private:
	TrackManager* m_trackManager{nullptr};
	SignalManager* m_signalManager{nullptr};

	TrackRenderType m_currentTrackRenderer{TrackRenderType::Default};
};
