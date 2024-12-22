//
// Created by arthur on 05/03/2023.
//

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>

#include <coreclr_delegates.h>
#include <semver.hpp>
#include <nethost.h>

#include "Concerto/Dotnet/HostFXR.hpp"
#include "Concerto/Dotnet/Assembly.hpp"

#ifdef CCT_PLATFORM_WINDOWS
#include <cwchar>
#endif // CCT_PLATFORM_WINDOWS


namespace cct::dotnet
{
	HostFXR::HostFXR(std::string path, std::string dotnetRuntimeConfigPath) :
		_hostfxrHandle(nullptr),
		_path(std::move(path)),
		_dotnetRuntimeConfigPath(std::move(dotnetRuntimeConfigPath))
	{
		if (!LoadHostFxr())
			CCT_ASSERT_FALSE("ConcertoDotNet: Failed to load hostfxr");
	}

	HostFXR::~HostFXR()
	{
		CloseHost();
	}

	Result<bool, std::string> HostFXR::LoadHostFxr()
	{
		using namespace std::string_literals;
		const std::filesystem::path hostFxrPath = GetHostFxrPath();
		if (hostFxrPath.empty())
			return "Could not find Hostfxr path"s;

		if (_hostfxrLib.IsLoaded())
		{
			CCT_ASSERT_FALSE("ConcertoDotNet: HostFXR already loaded");
			return "HostFXR already loaded"s;
		}

		if (_hostfxrLib.Load(hostFxrPath) == false)
			return std::format("Could not load library '{}'", hostFxrPath.string());

		auto hostfxrGetDotnetEnvironmentInfo = _hostfxrLib.GetFunction<Int32, const char_t*, void*, hostfxr_get_dotnet_environment_info_result_fn,void*>("hostfxr_get_dotnet_environment_info");
		if (!hostfxrGetDotnetEnvironmentInfo)
		{
			CCT_ASSERT_FALSE("ConcertoDotNet: Could not find hostfxr_get_dotnet_environment_info");
			return "Could not find hostfxr_get_dotnet_environment_info"s;
		}
#if 0
#else
		auto callback = [](const struct hostfxr_dotnet_environment_info* info, void* hostFxr) -> void
		{
			HostFXR* host = static_cast<HostFXR*>(hostFxr);
			if (!host)
			{
				CCT_ASSERT_FALSE("ConcertoDotNet: Invalid pointer");
				return;
			}
			std::span sdks(info->sdks, info->sdk_count);

			String highestSdkPath;
			semver::version highestSdkVersion;
			for (const auto& sdk : sdks)
			{
			#ifdef CCT_PLATFORM_WINDOWS
				std::string buff;
				buff.resize(std::wcslen(sdk.version));
				std::wcstombs(buff.data(), sdk.version, buff.size());
				semver::version version(buff);
			#else
				semver::version version(sdk.version);
			#endif
				if (version > highestSdkVersion)
				{
					highestSdkVersion = version;
					highestSdkPath = sdk.path;
				}
			}
			cct::Logger::Info("ConcertoDotnet: Selected SDK: {}", highestSdkVersion.to_string());
			host->SetSdkPath(std::move(highestSdkPath));
		};
		hostfxrGetDotnetEnvironmentInfo(nullptr, nullptr, callback, this);
#endif
		InitializeHost();
		return true;
	}

	std::string HostFXR::GetHostFxrPath() const
	{
#ifdef CCT_HOSTFXR_PATH
		CCT_ASSERT(std::filesystem::exists(CCT_HOSTFXR_PATH), "ConcertoDotNet: hostfxr path does not exist");
		return CCT_HOSTFXR_PATH;
#else
		char_t buffer[260];
		size_t bufferSize = std::size(buffer);
		const int rc = get_hostfxr_path(buffer, &bufferSize, nullptr);
		if (rc != 0)
		{
			CCT_ASSERT_FALSE("ConcertoDotNet: cannot retrieve hostfxr path error code : {}", rc);
			return {};
		}
		std::string res;
		res.resize(bufferSize);
		std::copy_n(buffer, bufferSize, res.begin());
		return res;
#endif
	}

	Result<bool, std::string> HostFXR::InitializeHost()
	{
		hostFxrInitializeHosfxr = _hostfxrLib.GetFunction<Int32, const char_t*, const hostfxr_initialize_parameters*, hostfxr_handle*>("hostfxr_initialize_for_runtime_config");
		hostFxrClose = _hostfxrLib.GetFunction<Int32, const hostfxr_handle>("hostfxr_close");
		hostFxrGetRuntimeDelegate = _hostfxrLib.GetFunction<Int32, const hostfxr_handle, hostfxr_delegate_type, void**>("hostfxr_get_runtime_delegate");

		std::filesystem::path path = _path;
		path = path / _dotnetRuntimeConfigPath;

#ifdef CCT_PLATFORM_WINDOWS
		const auto platformPath = path.wstring();
#else
		const auto platformPath = path.string();
#endif

		const char_t* str = platformPath.c_str();
		const int result = hostFxrInitializeHosfxr(str, nullptr, &_hostfxrHandle);
		if (_hostfxrHandle == nullptr || result != 0)
		{
			CCT_ASSERT_FALSE("ConcertoDotNet: could not initialize host error code: {}", result);
			return std::format("Could not initialize host error code: {}", result);
		}
		return true;
	}

	void HostFXR::CloseHost()
	{
		hostFxrClose(_hostfxrHandle);
	}

	void HostFXR::InitializeAndStartDotNetRuntime()
	{
		//void* function = nullptr;
		//const int result = hostFxrGetDotNetDelegate(_hostfxrHandle, hdt_load_in_memory_assembly, &function);
		//if (result != 0)
		//{
		//	CCT_ASSERT_FALSE("ConcertoDotNet: could not ");
		//}
	}

	Result<Int32, std::string> HostFXR::LoadDotNetAssembly(Assembly& assembly, const std::string& assemblyPath, std::string assemblyName)
	{
		std::filesystem::path path = _path;
		path = path / assemblyPath;
		void* function = nullptr;

		int result = hostFxrGetRuntimeDelegate(_hostfxrHandle, hdt_load_assembly, &function);
		if (result != 0)
		{
			CCT_ASSERT_FALSE("ConcertoDotNet: could not get runtime delegate for hdt_load_assembly (path '{}'), error code : {}", assemblyPath, result);
			return std::format("ConcertoDotNet: could not get runtime delegate for hdt_load_assembly (path '{}'), error code : {}", assemblyPath, result);
		}
		assembly._loadAssembly = reinterpret_cast<load_assembly_fn>(function);

#ifdef CCT_PLATFORM_WINDOWS
		auto platformPath = path.wstring();
		String wAssemblyName(assemblyName.begin(), assemblyName.end());
		assembly.Construct(std::move(platformPath), std::move(wAssemblyName));
#else
		auto platformPath = path.string();
		assembly.Construct(std::move(platformPath), std::move(assemblyName));
#endif

		function = nullptr;

		result = hostFxrGetRuntimeDelegate(_hostfxrHandle, hdt_get_function_pointer, &function);
		if (result != 0)
		{
			CCT_ASSERT_FALSE("ConcertoDotNet: could get runtime delegate for hdt_get_function_pointer (path '{}'), error code : {}", assemblyPath, result);
			return std::format("ConcertoDotNet: could get runtime delegate for hdt_get_function_pointer (path '{}'), error code : {}", assemblyPath, result);
		}
		assembly._getFunctionPointer = reinterpret_cast<get_function_pointer_fn>(function);

		CloseHost();

		return {0};
	}

	void HostFXR::SetSdkPath(String path)
	{
		_sdkPath = std::move(path);
	}
} // Concerto