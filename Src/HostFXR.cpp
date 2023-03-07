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

namespace Concerto::DotNet
{
	HostFXR::HostFXR(std::string path, std::string dotnetRuntimeConfigPath)
		: _path(std::move(path)), _hostfxrHandle(nullptr), _dotnetRuntimeConfigPath(std::move(dotnetRuntimeConfigPath))
	{
		if (!LoadHostFxr())
			throw std::runtime_error("Failed to load hostfxr");
		InitializeHost();
	}

	HostFXR::~HostFXR()
	{
		CloseHost();
	}

	bool HostFXR::LoadHostFxr()
	{
		try
		{
			std::filesystem::path hostFxrPath = GetHostFxrPath();
			_hostfxrLib = dylib(hostFxrPath.parent_path(),hostFxrPath.filename().string(), false);
		}
		catch (const dylib::load_error& e)
		{
			std::cerr << e.what() << std::endl;
			return false;
		}
		catch (const dylib::symbol_error& e)
		{
			std::cerr << e.what() << std::endl;
			return false;
		}
		return true;
	}

	std::string HostFXR::GetHostFxrPath() const
	{
		char_t buffer[260];
		size_t bufferSize = sizeof(buffer) / sizeof(char_t);
		int rc = get_hostfxr_path(buffer, &bufferSize, nullptr);
		if (rc != 0)
			return {};
		std::string res;
		res.resize(bufferSize);
		std::copy(buffer, buffer + bufferSize, res.begin());
		return res;
	}

	void HostFXR::InitializeHost()
	{
		try
		{
			init_fptr = _hostfxrLib->get_function < std::int32_t(
			const char_t *, const struct hostfxr_initialize_parameters* parameters, hostfxr_handle
			*host_context_handle)>("hostfxr_initialize_for_runtime_config");

			close_fptr = _hostfxrLib->get_function < std::int32_t(
			const hostfxr_handle host_context_handle)>("hostfxr_close");

			get_delegate_fptr = _hostfxrLib->get_function < std::int32_t(
			const hostfxr_handle host_context_handle,
			enum hostfxr_delegate_type type,
			void** delegate)>("hostfxr_get_runtime_delegate");
		}
		catch (const dylib::symbol_error& e)
		{
			std::cerr << e.what() << std::endl;
			throw;
		}
		std::filesystem::path path = _path;
		path = path / _dotnetRuntimeConfigPath;
		int result = init_fptr((char_t*)path.wstring().c_str(), nullptr, &_hostfxrHandle);
		if (_hostfxrHandle == nullptr || result != 0)
			throw std::runtime_error("Failed to initialize hostfxr");
	}

	void HostFXR::CloseHost()
	{
		close_fptr(_hostfxrHandle);
	}

	void HostFXR::InitializeAndStartDotNetRuntime()
	{
		void *function = nullptr;
		int result = get_delegate_fptr(_hostfxrHandle, hdt_load_in_memory_assembly, &function);
		if (result != 0)
			throw std::runtime_error("Failed to get delegate");

	}

	Assembly HostFXR::LoadDotNetAssembly(std::string assemblyPath, std::string assemblyName)
	{
		std::filesystem::path path = _path;
		path = path / assemblyPath;
		Assembly assembly(path.string(), std::move(assemblyName));
		void* function = nullptr;
		int result = get_delegate_fptr(_hostfxrHandle, hdt_load_assembly_and_get_function_pointer, &function);
		if (result != 0)
			throw std::runtime_error("Failed to get delegate");
		assembly._load_assembly_and_get_function_pointer =
			reinterpret_cast<load_assembly_and_get_function_pointer_fn>(function);
		return std::move(assembly);
	}
} // Concerto