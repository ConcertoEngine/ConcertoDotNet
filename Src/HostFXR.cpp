//
// Created by arthur on 05/03/2023.
//

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <coreclr_delegates.h>

#include "HostFXR.hpp"
#include "nethost.h"
#include "Assembly.hpp"

namespace cct::dotnet
{
	HostFXR::HostFXR(std::string path, std::string dotnetRuntimeConfigPath) :
		_hostfxrHandle(nullptr),
		_path(std::move(path)),
		_dotnetRuntimeConfigPath(std::move(dotnetRuntimeConfigPath))
	{
		if (LoadHostFxr())
			InitializeHost();
		else CCT_ASSERT_FALSE("ConcertoDotNet: Failed to load hostfxr");
	}

	HostFXR::~HostFXR()
	{
		CloseHost();
	}

	bool HostFXR::LoadHostFxr()
	{
		const std::filesystem::path hostFxrPath = GetHostFxrPath();
		if (hostFxrPath.empty())
			return false;
		_hostfxrLib.Load(hostFxrPath);
		return _hostfxrLib.IsLoaded();
	}

	std::string HostFXR::GetHostFxrPath() const
	{
		char_t buffer[260];
		size_t bufferSize = std::size(buffer);
		const int rc = get_hostfxr_path(buffer, &bufferSize, nullptr);
		if (rc != 0)
		{
			CCT_ASSERT_FALSE("ConcertoDotNet: cannot retreive hostfxr path error code : {}", rc);
			return {};
		}
		std::string res;
		res.resize(bufferSize);
		std::copy_n(buffer, bufferSize, res.begin());
		return res;
	}

	void HostFXR::InitializeHost()
	{
		init_fptr = _hostfxrLib.GetFunction<Int32, const char_t*, const hostfxr_initialize_parameters*, hostfxr_handle*>("hostfxr_initialize_for_runtime_config");
		close_fptr = _hostfxrLib.GetFunction<Int32, const hostfxr_handle>("hostfxr_close");
		get_delegate_fptr = _hostfxrLib.GetFunction<Int32, const hostfxr_handle, hostfxr_delegate_type, void**>("hostfxr_get_runtime_delegate");
		std::filesystem::path path = _path;
		path = path / _dotnetRuntimeConfigPath;

#ifdef CCT_PLATFORM_WINDOWS
		const auto wString = path.wstring();
		const char_t* str = wString.c_str();
#else
		const auto string = path.string();
		const char_t* str = string.c_str();
#endif

		const int result = init_fptr(str, nullptr, &_hostfxrHandle);
		if (_hostfxrHandle == nullptr || result != 0)
			CCT_ASSERT_FALSE("ConcertoDotNet: could not initalize host error code: {}", result);
	}

	void HostFXR::CloseHost()
	{
		close_fptr(_hostfxrHandle);
	}

	void HostFXR::InitializeAndStartDotNetRuntime()
	{
		void* function = nullptr;
		const int result = get_delegate_fptr(_hostfxrHandle, hdt_load_in_memory_assembly, &function);
		if (result != 0)
		{
			CCT_ASSERT_FALSE("ConcertoDotNet: could not ");
		}
	}

	Result<Int32, std::string> HostFXR::LoadDotNetAssembly(Assembly& assembly, const std::string& assemblyPath, std::string assemblyName)
	{
		std::filesystem::path path = _path;
		path = path / assemblyPath;
		assembly.Construct(path.string(), std::move(assemblyName));
		void* function = nullptr;
		const int result = get_delegate_fptr(_hostfxrHandle, hdt_load_assembly_and_get_function_pointer, &function);
		if (result != 0)
		{
			CCT_ASSERT_FALSE("ConcertoDotNet: could not load assembly (path '{}', name {}), error code : {}", assemblyPath, assembly._assemblyName, result);
			return std::format("ConcertoDotNet: could not load assembly (path '{}', name {}), error code : {}", assemblyPath, assembly._assemblyName, result);
		}
		assembly._load_assembly_and_get_function_pointer = reinterpret_cast<load_assembly_and_get_function_pointer_fn>(function);
		CloseHost();
		return {0};
	}
} // Concerto