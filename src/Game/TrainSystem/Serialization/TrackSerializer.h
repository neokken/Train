#pragma once

#include <json.hpp>
#include "game/TrainSystem/TrackSegment.h"

// ** type Serialization **

// ** Track Node id Serialization **
void to_json( nlohmann::json& j, const TrackNodeID& id );
void from_json( const nlohmann::json& j, TrackNodeID& id );

// ** Segment Node id Serialization **
void to_json( nlohmann::json& j, const TrackSegmentID& id );
void from_json( const nlohmann::json& j, TrackSegmentID& id );

// ** DATA Serialization **

// ** Track Node Serialization **
void to_json( nlohmann::json& j, const TrackNode& node );
void from_json( const nlohmann::json& j, TrackNode& node );

// ** Track Segment Serialization **
void to_json( nlohmann::json& j, const TrackSegment& segment );
void from_json( const nlohmann::json& j, TrackSegment& segment );
