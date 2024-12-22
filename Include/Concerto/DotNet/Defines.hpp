//
// Created by arthur on 24/06/2024.
//

#ifndef CONCERTO_DOTNET_DEFINES_HPP
#define CONCERTO_DOTNET_DEFINES_HPP

#include <Concerto/Core/Types.hpp>

#ifdef CCT_COMPILER_MSVC
#pragma warning(disable: 4251) // Disable warning about DLL interface needed
#endif

#ifdef CCT_DOTNET_BUILD
	#define CCT_DOTNET_API CCT_EXPORT
#else
	#define CCT_DOTNET_API CCT_IMPORT
#endif // CCT_DOTNET_BUILD

#ifdef CCT_PLATFORM_WINDOWS
#define CCT_AUTO_WIDE_STRING(str) L##str
#else
#define CCT_AUTO_WIDE_STRING(str) str
#endif // CCT_PLATFORM_WINDOWS


#include <hostfxr.h>

namespace cct::dotnet
{
	using String = std::basic_string<char_t>;
}

#endif // CONCERTO_DOTNET_DEFINES_HPP