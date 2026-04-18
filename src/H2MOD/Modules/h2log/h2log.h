#pragma once

#ifndef SPDLOG_DISABLED

// Disable sign/unsigned comparison mismatch in spdlog
#pragma warning( push )
#pragma warning( disable : 4389)
#pragma warning( disable : 4244)
#include "spdlog/logger.h"
#pragma warning( pop )

enum log_level : unsigned int {
	_trace,    //          Tell me *everything*
	_debug,    //          Give me the dirty details
	_info,     // Default. Occasionally helpful information
	_warning,  //          What probably shouldn't be happening
	_error,    //          Bad news only, please
	_critical  //          I only want to see death and destruction
};

class h2log
{
private:
	h2log(const std::string& name);
	h2log() = delete;

public:
	~h2log();

	// Good for checking if the logger is actually able to ouput logs
	bool is_valid() const;

	/// <summary>
	///   <para>Creates a logger which outputs to a file.</para>
	///   <para>Use logger.is_valid() to check if logging is working.</para>
	/// </summary>
	static h2log* create(const std::string &name, const std::wstring &filename, int debugLogLevel);

	/// <summary>
	///   <para>Creates a logger which outputs to a console window.</para>
	///   <para>Use logger.is_valid() to check if logging is working.</para>
	/// </summary>
	static h2log* create_console(const std::string &name, int debugLogLevel);

	// For the most unimportant stuff
	template<typename... Args>
	void output(log_level level, const std::string& fmt, const Args &... args) {
		if (m_output != nullptr)
			m_output->log((spdlog::level::level_enum)level, fmt.data(), std::forward<const Args&>(args)...);
		if (!isConsole && console != nullptr && console->m_output != nullptr)
			console->m_output->log((spdlog::level::level_enum)level, ("[" + m_sname + "] " + fmt).data(), std::forward<const Args&>(args)...);
	}

	// For the most unimportant stuff
	template<typename... Args>
	void output(log_level level, const std::wstring& fmt, const Args &... args) { 
		if (m_output != nullptr)
			m_output->log((spdlog::level::level_enum)level, fmt.data(), std::forward<const Args&>(args)...);
		if (!isConsole && console != nullptr && console->m_output != nullptr)
			console->m_output->log((spdlog::level::level_enum)level, (L"[" + m_wname + L"] " + fmt).data(), std::forward<const Args&>(args)...);
	}

public:
	bool isConsole = false;

private:
	static bool failAlerted;
	static h2log* console;

	std::string m_sname;
	std::wstring m_wname;
	std::shared_ptr<spdlog::logger> m_output = nullptr;
};

extern h2log* g_xlive_log;
extern h2log* g_h2mod_log;
extern h2log* g_network_log;
extern h2log* g_console_log;
extern h2log* g_onscreendebug_log;

template<typename... Args>
static inline void h2log_print(h2log* logger, log_level level, const char* msg, const Args &... args)
{
	if (logger != NULL)
	{
		logger->output(level, msg, std::forward<const Args&>(args)...);
	}
}

template<typename... Args>
static inline void h2log_print(h2log* logger, log_level level, const wchar_t* msg, const Args &... args)
{
	if (logger != NULL)
	{
		logger->output(level, msg, std::forward<const Args&>(args)...);
	}
}

// to note this is not thread safe
// you might see the max log count reached message more than once in a row
#define LIMITED_LOG(log_limit, logger, msg, ...) \
do \
{ \
	static unsigned int _logged_times_count; \
	if (_logged_times_count < log_limit) { \
		logger(msg, __VA_ARGS__); \
		_logged_times_count++; \
	} \
	else if (_logged_times_count == log_limit) \
	{ \
		logger("	reached max log count of {} for: ", log_limit); \
		logger(msg, __VA_ARGS__); \
		_logged_times_count++; \
	} \
} while(0)

#define LIMITED_LOGW(log_limit, logger, ...) \
do \
{ \
	static unsigned int _logged_times_count; \
	if (_logged_times_count < log_limit) { \
		logger(__VA_ARGS__); \
		_logged_times_count++; \
	} \
	else if (_logged_times_count == log_limit) \
	{ \
		logger(L"	reached max log count of {} for: ", log_limit); \
		logger(__VA_ARGS__); \
		_logged_times_count++; \
	} \
} while(0)

// Generic logging
// For the most unimportant stuff
#define LOG_TRACE(logger, msg, ...)      h2log_print(logger, log_level::_trace, msg, __VA_ARGS__)

// Somewhat more useful information
#define LOG_DEBUG(logger, msg, ...)      h2log_print(logger, log_level::_debug, msg, __VA_ARGS__)

// Things that even users may want to see
#define LOG_INFO(logger, msg, ...)       h2log_print(logger, log_level::_info, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one
#define LOG_WARNING(logger, msg, ...)    h2log_print(logger, log_level::_warning, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events
#define LOG_ERROR(logger, msg, ...)      h2log_print(logger, log_level::_error,  msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke
#define LOG_CRITICAL(logger, msg, ...)   h2log_print(logger, log_level::_critical, msg, __VA_ARGS__)

// Mod-specific logging
// For the most unimportant stuff related to H2mod specifically
#define LOG_TRACE_GAME(msg, ...)         LOG_TRACE     (g_h2mod_log, msg, __VA_ARGS__)

// Somewhat more useful information related to H2mod specifically
#define LOG_DEBUG_GAME(msg, ...)         LOG_DEBUG     (g_h2mod_log, msg, __VA_ARGS__)

// Things that even users may want to see related to H2mod specifically
#define LOG_INFO_GAME(msg, ...)          LOG_INFO      (g_h2mod_log, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one related to H2mod specifically
#define LOG_WARNING_GAME(msg, ...)       LOG_WARNING   (g_h2mod_log, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events related to H2mod specifically
#define LOG_ERROR_GAME(msg, ...)         LOG_ERROR     (g_h2mod_log, msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke related to H2mod specifically
#define LOG_CRITICAL_GAME(msg, ...)      LOG_CRITICAL  (g_h2mod_log, msg, __VA_ARGS__)

// Function calls
// For the most unimportant stuff, prepends the current function name
#define LOG_TRACE_FUNC(msg, ...)         LOG_TRACE     (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// For the most unimportant stuff, prepends the current function name
#define LOG_TRACE_FUNCW(msg, ...)        LOG_TRACE     (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// Somewhat more useful information, prepends the current function name
#define LOG_DEBUG_FUNC(msg, ...)         LOG_DEBUG     (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// Somewhat more useful information, prepends the current function name
#define LOG_DEBUG_FUNCW(msg, ...)        LOG_DEBUG     (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// Things that even users may want to see, prepends the current function name
#define LOG_INFO_FUNC(msg, ...)          LOG_INFO      (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// Things that even users may want to see, prepends the current function name
#define LOG_INFO_FUNCW(msg, ...)         LOG_INFO      (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// A surprise to be sure, but not a serious one, prepends the current function name
#define LOG_WARNING_FUNC(msg, ...)       LOG_WARNING   (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// A surprise to be sure, but not a serious one, prepends the current function name
#define LOG_WARNING_FUNCW(msg, ...)      LOG_WARNING   (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// Absolutely not good, probably game breaking events, prepends the current function name
#define LOG_ERROR_FUNC(msg, ...)         LOG_ERROR     (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// Absolutely not good, probably game breaking events, prepends the current function name
#define LOG_ERROR_FUNCW(msg, ...)        LOG_ERROR     (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke, prepends the current function name
#define LOG_CRITICAL_FUNC(msg, ...)      LOG_CRITICAL  (g_h2mod_log, "{}(): " msg, __func__, __VA_ARGS__)
// "Wait, that's illegal" except it is definitely not a joke, prepends the current function name
#define LOG_CRITICAL_FUNCW(msg, ...)     LOG_CRITICAL  (g_h2mod_log, L"{}(): " msg, __FUNCTIONW__, __VA_ARGS__)

// Networking
// For the most unimportant stuff related to networking
#define LOG_TRACE_NETWORK(msg, ...)      LOG_TRACE     (g_network_log, msg, __VA_ARGS__)

// Somewhat more useful information related to networking
#define LOG_DEBUG_NETWORK(msg, ...)      LOG_DEBUG     (g_network_log, msg, __VA_ARGS__)

// Things that even users may want to see related to networking
#define LOG_INFO_NETWORK(msg, ...)       LOG_INFO      (g_network_log, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one related to networking
#define LOG_WARNING_NETWORK(msg, ...)    LOG_WARNING   (g_network_log, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events related to networking
#define LOG_ERROR_NETWORK(msg, ...)      LOG_ERROR     (g_network_log, msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke related to networking
#define LOG_CRITICAL_NETWORK(msg, ...)   LOG_CRITICAL  (g_network_log, msg, __VA_ARGS__)

// xLiveLess
// For the most unimportant stuff related to xLiveLess
#define LOG_TRACE_XLIVE(msg, ...)        LOG_TRACE     (g_xlive_log, msg, __VA_ARGS__)

// Somewhat more useful information related to xLiveLess
#define LOG_DEBUG_XLIVE(msg, ...)        LOG_DEBUG     (g_xlive_log, msg, __VA_ARGS__)

// Things that even users may want to see related to xLiveLess
#define LOG_INFO_XLIVE(msg, ...)         LOG_INFO      (g_xlive_log, msg, __VA_ARGS__)

// A surprise to be sure, but not a serious one related to xLiveLess
#define LOG_WARNING_XLIVE(msg, ...)      LOG_WARNING   (g_xlive_log, msg, __VA_ARGS__)

// Absolutely not good, probably game breaking events related to xLiveLess
#define LOG_ERROR_XLIVE(msg, ...)        LOG_ERROR     (g_xlive_log, msg, __VA_ARGS__)

// "Wait, that's illegal" except it is definitely not a joke related to xLiveLess
#define LOG_CRITICAL_XLIVE(msg, ...)     LOG_CRITICAL  (g_xlive_log, msg, __VA_ARGS__)

#else

template<typename... Args>
void h2log_stub(const std::string& fmt, const Args &... args) { SUPRESS_UNUSED(fmt); }

template<typename... Args>
static void h2log_stub(const std::wstring& fmt, const Args &... args) { SUPRESS_UNUSED(fmt); }

#define LOG_STUB(...)					\
if (0)									\
{										\
	h2log_stub(__VA_ARGS__);	\
} (void)0

#define LIMITED_LOG(log_limit, logger, ...) \
if (0)										\
{											\
	log_limit;								\
	LOG_STUB(__VA_ARGS__);					\
} (void)0

#define LOG_TRACE(...) LOG_STUB(__VA_ARGS__)
#define LOG_DEBUG(...) LOG_STUB(__VA_ARGS__)
#define LOG_INFO(...) LOG_STUB(__VA_ARGS__)
#define LOG_WARNING(...) LOG_STUB(__VA_ARGS__)
#define LOG_ERROR(...) LOG_STUB(__VA_ARGS__)
#define LOG_CRITICAL(...) LOG_STUB(__VA_ARGS__)

// Mod-specific logging
#define LOG_TRACE_GAME(...) LOG_STUB(__VA_ARGS__)
#define LOG_DEBUG_GAME(...) LOG_STUB(__VA_ARGS__)
#define LOG_INFO_GAME(...) LOG_STUB(__VA_ARGS__)
#define LOG_WARNING_GAME(...) LOG_STUB(__VA_ARGS__)
#define LOG_ERROR_GAME(...) LOG_STUB(__VA_ARGS__)
#define LOG_CRITICAL_GAME(...) LOG_STUB(__VA_ARGS__)

// Function calls
#define LOG_TRACE_FUNC(...) LOG_STUB(__VA_ARGS__)
#define LOG_TRACE_FUNCW(...) LOG_STUB(__VA_ARGS__)
#define LOG_DEBUG_FUNC(...) LOG_STUB(__VA_ARGS__)
#define LOG_DEBUG_FUNCW(...) LOG_STUB(__VA_ARGS__)
#define LOG_INFO_FUNC(...) LOG_STUB(__VA_ARGS__)
#define LOG_INFO_FUNCW(...) LOG_STUB(__VA_ARGS__)
#define LOG_WARNING_FUNC(...) LOG_STUB(__VA_ARGS__)
#define LOG_WARNING_FUNCW(...) LOG_STUB(__VA_ARGS__)
#define LOG_ERROR_FUNC(...) LOG_STUB(__VA_ARGS__)
#define LOG_ERROR_FUNCW(...) LOG_STUB(__VA_ARGS__)
#define LOG_CRITICAL_FUNC(...) LOG_STUB(__VA_ARGS__)
#define LOG_CRITICAL_FUNCW(...) LOG_STUB(__VA_ARGS__)

// Networking
#define LOG_TRACE_NETWORK(...) LOG_STUB(__VA_ARGS__)
#define LOG_DEBUG_NETWORK(...) LOG_STUB(__VA_ARGS__)
#define LOG_INFO_NETWORK(...) LOG_STUB(__VA_ARGS__)
#define LOG_WARNING_NETWORK(...) LOG_STUB(__VA_ARGS__)
#define LOG_ERROR_NETWORK(...) LOG_STUB(__VA_ARGS__)
#define LOG_CRITICAL_NETWORK(...) LOG_STUB(__VA_ARGS__)

// xLiveLess
#define LOG_TRACE_XLIVE(...) LOG_STUB(__VA_ARGS__)
#define LOG_DEBUG_XLIVE(...) LOG_STUB(__VA_ARGS__)
#define LOG_INFO_XLIVE(...) LOG_STUB(__VA_ARGS__)
#define LOG_WARNING_XLIVE(...) LOG_STUB(__VA_ARGS__)
#define LOG_ERROR_XLIVE(...) LOG_STUB(__VA_ARGS__)
#define LOG_CRITICAL_XLIVE(...) LOG_STUB(__VA_ARGS__)

#define LOG_CHECK(...)					\
if (0)									\
{										\
	h2log_stub(__VA_ARGS__);			\
} (void)0

#endif

void h2log_initialize();
void h2log_dispose();
