#include "precomp.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Engine {
	std::shared_ptr<spdlog::logger> Logger::s_engineLogger;
	std::shared_ptr<spdlog::logger> Logger::s_gameLogger;
}

void Engine::Logger::Init()
{
	std::vector<spdlog::sink_ptr> logSinks;
	logSinks.emplace_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>() );
	logSinks.emplace_back( std::make_shared<spdlog::sinks::basic_file_sink_mt>( "Hazel.log", true ) );

	logSinks[0]->set_pattern( "%^[%T] %n: %v%$" );
	logSinks[1]->set_pattern( "[%T] [%l] %n: %v" );

	s_engineLogger = std::make_shared<spdlog::logger>( "Engine", begin( logSinks ), end( logSinks ) );
	spdlog::register_logger( s_engineLogger );
	s_engineLogger->set_level( spdlog::level::trace );
	s_engineLogger->flush_on( spdlog::level::trace );

	s_gameLogger = std::make_shared<spdlog::logger>( "Game", begin( logSinks ), end( logSinks ) );
	spdlog::register_logger( s_gameLogger );
	s_gameLogger->set_level( spdlog::level::trace );
	s_gameLogger->flush_on( spdlog::level::trace );
}