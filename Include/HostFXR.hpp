//
// Created by arthur on 05/03/2023.
//

#ifndef CONCERTOCLR_INCLUDE_HOSTFXR_HPP_
#define CONCERTOCLR_INCLUDE_HOSTFXR_HPP_

#include <functional>
#include <string>
#include <optional>

#include "HostFxr.h"
#include "dylib.hpp"
#include "Assembly.hpp"

namespace Concerto::DotNet
{
	class HostFXR
	{
	 public:
		explicit HostFXR(std::string path, std::string dotnetRuntimeConfigPath);
		~HostFXR();
		[[nodiscard]] std::string GetHostFxrPath() const;
		void InitializeHost();
		void CloseHost();
		void InitializeAndStartDotNetRuntime();
		Assembly LoadDotNetAssembly(std::string assemblyPath, std::string assemblyName);
	 private:
		bool LoadHostFxr();
		std::function<std::int32_t(const char_t *, const struct hostfxr_initialize_parameters *, hostfxr_handle *)> init_fptr;
		std::function<std::int32_t(const hostfxr_handle, enum hostfxr_delegate_type, void **)> get_delegate_fptr;
		std::function<std::int32_t(const char_t *, const char_t *, const char_t *, const char_t *, void  *, void **)> dotnet_assembly;
		std::function<std::int32_t(const hostfxr_handle)> close_fptr;

		hostfxr_handle _hostfxrHandle;
		std::optional<dylib> _hostfxrLib;
		std::string _path;
		std::string _dotnetRuntimeConfigPath;
	};

} // Concerto

#endif //CONCERTOCLR_INCLUDE_HOSTFXR_HPP_
