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

namespace cct::dotnet
{
	class CCT_DOTNET_API HostFXR
	{
	 public:
		explicit HostFXR(std::string path, std::string dotnetRuntimeConfigPath);
		~HostFXR();

		[[nodiscard]] std::string GetHostFxrPath() const;

		Result<bool, std::string> InitializeHost();
		void CloseHost();
		void InitializeAndStartDotNetRuntime();

		Result<Int32, std::string> LoadDotNetAssembly(Assembly& assembly, const std::string &assemblyPath, std::string assemblyName);

		void SetSdkPath(String path);
	 private:
		Result<bool, std::string> LoadHostFxr();
		FunctionRef<Int32(const char_t *, const hostfxr_initialize_parameters *, hostfxr_handle *)> hostFxrInitializeHosfxr;
		FunctionRef<Int32(const hostfxr_handle, hostfxr_delegate_type, void **)> hostFxrGetRuntimeDelegate;
		FunctionRef<Int32(const char_t *, const char_t *, const char_t *, const char_t *, void  *, void **)> hostFxrDotnetAssembly;
		FunctionRef<Int32(const hostfxr_handle)> hostFxrClose;

		hostfxr_handle _hostfxrHandle;
		DynLib _hostfxrLib;
		std::string _path;
		std::string _dotnetRuntimeConfigPath;
		String _sdkPath;
	};

} // Concerto::DotNet

#endif //CONCERTO_DOTNET_HOSTFXR_HPP
