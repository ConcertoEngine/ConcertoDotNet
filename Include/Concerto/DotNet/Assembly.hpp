//
// Created by arthur on 05/03/2023.
//

#ifndef CONCERTO_DOTNET_ASSEMBLY_HPP
#define CONCERTO_DOTNET_ASSEMBLY_HPP

#include <string>
#include <unordered_map>
#include <filesystem>
#include <type_traits>

#include <Concerto/Core/FunctionRef.hpp>
#include <Concerto/Core/Assert.hpp>
#include <hostfxr.h>

#include "Concerto/DotNet/Defines.hpp"

namespace Concerto::DotNet
{
	/**
	 * @brief A class that represents a .NET assembly
	 */
	class CONCERTO_DOTNET_API Assembly
	{
	public:
		friend class HostFXR;
		/**
		 * @brief Construct a new Dot Net Assembly object
		 * @param assemblyPath The path to the assembly file
		 * @param assemblyName The name of the assembly
		 */
		Assembly(std::string assemblyPath, std::string assemblyName) :
			_assemblyPath(std::move(assemblyPath)),
			_assemblyName(std::move(assemblyName)),
			_load_assembly_and_get_function_pointer()
		{
		}

		/**
		 * @brief Invoke a function from the assembly with a return value
		 * @tparam T The function return type
		 * @param functionName The name of the function to invoke
		 * @param args The arguments to pass to the function
		 * @return The return value of the function signature
		 * @info The first call to this function (with the same arguments and return value) may be slower than
		 * subsequent calls, because the function is retrieved from the assembly
		 */
		template<typename T, typename ...Args>
		T Invoke(const std::string& functionName, Args&& ...args)
		{
			auto* functionPointer = GetFunctionPointer<T(Args...)>(functionName);
			if constexpr (std::is_void_v<T>)
			{
				functionPointer(std::forward<Args>(args)...);
				return;
			}
			else return functionPointer(std::forward<Args>(args)...);
		}

		/**
		 * @brief Invoke a function from the assembly, without a return value
		 * @param functionName The name of the function to invoke
		 * @param args The arguments to pass to the function
		 * @return The return value of the function signature
		 * @info The first call to this function (with the same arguments and return value) may be slower than
		 * subsequent calls, because the function is retrieved from the assembly
		 */
		template< typename ...Args>
		void Invoke(const std::string& functionName, Args&& ...args)
		{
			auto* functionPointer = GetFunctionPointer<void(Args...)>(functionName);
			functionPointer(std::forward<Args>(args)...);
		}
	private:

		/**
		 * @brief Check if the function pointer exists in the cache, if not, load it from the assembly
		 * @tparam T The function signature
		 * @param functionName The name of the function to get the pointer from
		 * @return The function pointer
		 */
		template<typename T>
		T* GetFunctionPointer(const std::string& functionName)
		{
			auto it = _loadedFunctions.find(functionName);
			if (it == _loadedFunctions.end())
			{
				T* functionPointer = GetFunctionPointerFromAssembly<T>(functionName);
				_loadedFunctions.emplace(functionName, functionPointer);
				return functionPointer;
			}
			return (T*)it->second;
		}

		/**
		 * @brief Get the function pointer from the assembly
		 * @tparam T The function signature
		 * @param functionName The name of the function to get the pointer from
		 * @return The function pointer
		 * @throw std::runtime_error if the function pointer could not be loaded
		 */
		template<typename T>
		T* GetFunctionPointerFromAssembly(const std::string& functionName)
		{
			std::string dotnetType = _assemblyName + ".Lib, " + _assemblyName;
			std::wstring wdotnetType(dotnetType.begin(), dotnetType.end()); //TODO: cross platform support
			std::wstring wfunctionName(functionName.begin(), functionName.end()); //TODO: cross platform support
			T* functionPointer = nullptr;
			const std::filesystem::path path = std::filesystem::current_path() / _assemblyPath;
			const int rc = _load_assembly_and_get_function_pointer(path.wstring().c_str(), wdotnetType.c_str(), wfunctionName.c_str(), nullptr, nullptr, (void**)&functionPointer);
			if (rc != 0)
			{
				CONCERTO_ASSERT_FALSE("ConcertoDotNet: Invalid return code {} -> {}", functionName, rc);
			}
			return functionPointer;
		}

		std::string _assemblyPath;
		std::string _assemblyName;
		std::unordered_map<std::string, void*> _loadedFunctions;
		FunctionRef<int(const char_t*, const char_t*, const char_t*, const char_t*, void*, void**)> _load_assembly_and_get_function_pointer;
	};
} // Concerto
#endif //CONCERTO_DOTNET_ASSEMBLY_HPP
