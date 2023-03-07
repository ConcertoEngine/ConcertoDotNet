//
// Created by arthur on 05/03/2023.
//

#ifndef CONCERTOCLR_INCLUDE_ASSEMBLY_HPP_
#define CONCERTOCLR_INCLUDE_ASSEMBLY_HPP_

#include <string>
#include <functional>
#include <unordered_map>
#include <filesystem>
#include <type_traits>

namespace Concerto::DotNet
{
	/**
	 * @brief A class that represents a .NET assembly
	 */
	class Assembly
	{
	 public:
		friend class HostFXR;
		/**
		 * @brief Construct a new Dot Net Assembly object
		 * @param assemblyPath The path to the assembly file
		 * @param assemblyName The name of the assembly
		 */
		Assembly(std::string assemblyPath, std::string assemblyName) :
			_assemblyPath(std::move(assemblyPath)), _assemblyName(std::move(assemblyName))
		{

		}

		/**
		 * @brief Invoke a function from the assembly
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
			std::wstring wdotnetType(dotnetType.begin(), dotnetType.end());
			std::wstring wfunctionName(functionName.begin(), functionName.end());
			T* functionPointer = nullptr;
			std::filesystem::path path = std::filesystem::current_path() / _assemblyPath;
			int rc = _load_assembly_and_get_function_pointer(
				reinterpret_cast<const char_t*>(path.wstring().c_str()),
				reinterpret_cast<const char_t*>(wdotnetType.c_str()),
				reinterpret_cast<const char_t*>(wfunctionName.c_str()),
				nullptr,
				nullptr,
				(void**)&functionPointer);
			if (rc != 0)
				throw std::runtime_error("Failed to load function pointer : '" + functionName + "', error code : " +
										 std::to_string(rc) );
			return functionPointer;
		}

		std::string _assemblyPath;
		std::string _assemblyName;
		std::unordered_map<std::string, void*> _loadedFunctions;
		std::function<int(const char_t*, const char_t*, const char_t*, const char_t*, void*, void**)>
			_load_assembly_and_get_function_pointer;
	};
} // Concerto
#endif //CONCERTOCLR_INCLUDE_ASSEMBLY_HPP_
