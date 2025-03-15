#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Engine
{
	class Logger
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_engineLogger; }
		static std::shared_ptr<spdlog::logger>& GetGameLogger() { return s_gameLogger; }

		template <typename... Args>
		static void Trace( fmt::format_string<Args...> fmt, Args&&... args )
		{
			s_engineLogger->trace(fmt, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void Info( fmt::format_string<Args...> fmt, Args&&... args )
		{
			s_engineLogger->info(fmt, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void Warn( fmt::format_string<Args...> fmt, Args&&... args )
		{
			s_engineLogger->warn(fmt, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void Error( fmt::format_string<Args...> fmt, Args&&... args )
		{
			s_engineLogger->error(fmt, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void Critical( fmt::format_string<Args...> fmt, Args&&... args )
		{
			s_engineLogger->critical(fmt, std::forward<Args>(args)...);
		}

	private:
		static std::shared_ptr<spdlog::logger> s_engineLogger;
		static std::shared_ptr<spdlog::logger> s_gameLogger;
	};
}
