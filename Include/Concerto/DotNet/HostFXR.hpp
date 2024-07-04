//
// Created by arthur on 05/03/2023.
//

#ifndef CONCERTO_DOTNET_HOSTFXR_HPP
#define CONCERTO_DOTNET_HOSTFXR_HPP

#include <functional>
#include <string>
#include <optional>

#include <Concerto/Core/DynLib.hpp>
#include <hostfxr.h>

#include "Concerto/DotNet/Defines.hpp"
#include "Concerto/DotNet/Assembly.hpp"

#ifdef CONCERTO_PLATFORM_WINDOWS
#define CONCERTO_AUTO_WIDE_STRING(str) L##str
#else
#define CONCERTO_AUTO_WIDE_STRING(str) str
#endif

namespace Concerto::DotNet
{
	class CONCERTO_DOTNET_API HostFXR
	{
	 public:
		explicit HostFXR(std::string path, std::string dotnetRuntimeConfigPath);
		~HostFXR();

		[[nodiscard]] std::string GetHostFxrPath() const;

		void InitializeHost();
		void CloseHost();
		void InitializeAndStartDotNetRuntime();

		Assembly LoadDotNetAssembly(const std::string &assemblyPath, std::string assemblyName);
	 private:
		bool LoadHostFxr();
		FunctionRef<Int32(const char_t *, const hostfxr_initialize_parameters *, hostfxr_handle *)> init_fptr;
		FunctionRef<Int32(const hostfxr_handle, hostfxr_delegate_type, void **)> get_delegate_fptr;
		FunctionRef<Int32(const char_t *, const char_t *, const char_t *, const char_t *, void  *, void **)> dotnet_assembly;
		FunctionRef<Int32(const hostfxr_handle)> close_fptr;

		hostfxr_handle _hostfxrHandle;
		DynLib _hostfxrLib;
		std::string _path;
		std::string _dotnetRuntimeConfigPath;
	};

} // Concerto::DotNet

#endif //CONCERTO_DOTNET_HOSTFXR_HPP
