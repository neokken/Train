// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

// common.h is to be included in host and device code and stores
// global settings and defines.

#pragma once

#include <numbers>

// default screen resolution
constexpr int SCRWIDTH = 1280;
constexpr int SCRHEIGHT = 720;
constexpr float ASPECT_RATIO = static_cast<float>(SCRWIDTH) / static_cast<float>(SCRHEIGHT);
// #define FULLSCREEN

//LOD in this context means amount of segments as part of a curve
constexpr float BASE_TRACK_LOD = 0.35f; // Base (sub)segments per distance for tracks
constexpr float BASE_SPOKE_LOD = 1.f; // Base (sub)segments per distance for track spokes, should be higher than track lod
constexpr uint MIN_SPOKE_LOD = 7u; // If spokes get bellow this LOD they stop drawing should be > 0
constexpr uint MAX_LOD = 100u;

//Physics
constexpr float WORLD_AIR_DENSITY = 0.01f; // Density of the air used for drag      | Should be a low value <0.1
constexpr float WORLD_TRACK_ROUGHNESS = 5000.f; // Used for drag experienced from track bends     | Should be a high value around ~1000-1000

//Visuals
constexpr bool SHOW_DRAG_PARTICLES = true;

// constants
#define PI			3.14159265358979323846264f
#define INVPI		0.31830988618379067153777f
#define INV2PI		0.15915494309189533576888f
#define TWOPI		6.28318530717958647692528f
#define SQRT_PI_INV	0.56418958355f
#define LARGE_FLOAT	1e34f

constexpr float golden_ratio = std::numbers::phi_v<float>;

enum class Color : std::uint8_t
{
	BackGround,
	BackGroundGrid1,
	BackGroundGrid2,

	TrackRail,
	TrackSpokes,

	TrackSelected,
	TrackSelectedInvalid,

	TrackNodeInfo,

	TrackHover_DEBUG,
	TrackSelect_DEBUG,
	TrackHoverSelect_DEBUG,
};

inline std::unordered_map<Color, uint> s_colorMap
{
	{Color::BackGround, 0x202020,},
	{Color::BackGroundGrid1, 0x303030},
	{Color::BackGroundGrid2, 0x404040},

	{Color::TrackRail, 0x95A3A4},
	{Color::TrackSpokes, 0x77625C},

	{Color::TrackSelected, 0x8CBA80},
	{Color::TrackSelectedInvalid, 0xDB504A},
	{Color::TrackNodeInfo, 0x0582CA},

	{Color::TrackHover_DEBUG, 0xFFD500},
	{Color::TrackSelect_DEBUG, 0xFF8C42},
	{Color::TrackHoverSelect_DEBUG, 0x00E5FF},

};

inline uint GetColor( const Color c )
{
	return s_colorMap[c];
}

namespace HeightLayer
{
	enum HeightLayer : std::uint8_t
	{
		Background = 0,
		Tracks = 10,
		Trains = 20,
		Default = 30,
		Buildings = 40,
		Debug = 80
	};
}

//Debug assert
#if _DEBUG
#define DEBUG_ASSERT(expression, message) if(!(expression)) printf("Assertion Failed:"#message"\n"), DebugBreak();
#else
#define DEBUG_ASSERT
#endif


//Debug assert
#if _DEBUG
#define DEBUG_ASSERT(expression, message) if(!(expression)) printf("Assertion Failed:"#message"\n"), DebugBreak();
#else
#define DEBUG_ASSERT
#endif


#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "Helpers/Helpers.h"

#include "Helpers/Logger.h"
