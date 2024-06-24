//
// Created by arthur on 24/06/2024.
//

#ifndef CONCERTO_DOTNET_DEFINES_HPP
#define CONCERTO_DOTNET_DEFINES_HPP

#include <Concerto/Core/Types.hpp>

#ifdef CONCERTO_COMPILER_MSVC
#pragma warning(disable: 4251) // Disable warning about DLL interface needed
#endif

#ifdef CONCERTO_DOTNET_BUILD
	#define CONCERTO_DOTNET_API CONCERTO_EXPORT
#else
	#define CONCERTO_DOTNET_API CONCERTO_IMPORT
#endif // CONCERTO_DOTNET_BUILD

#endif // CONCERTO_DOTNET_DEFINES_HPP