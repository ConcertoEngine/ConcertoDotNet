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
#include <Concerto/Reflection/Reflection.hpp>


#include "Concerto/DotNet/Defines.hpp"

namespace cct::dotnet
{
	/**
	 * @brief A class that represents a .NET assembly
	 */
	class CCT_DOTNET_API Assembly : public cct::refl::Object
	{
	public:
		friend class HostFXR;
		/**
		 * @brief Construct a new Dot Net Assembly object
		 * @param assemblyPath The path to the assembly file
		 * @param assemblyName The name of the assembly
		 */
		void Construct(String assemblyPath, String assemblyName)
		{
			_assemblyPath = std::move(assemblyPath);
			_assemblyName = std::move(assemblyName);
			auto res = _loadAssembly(_assemblyPath.c_str(), nullptr, nullptr);
			if (res != 0)
			{
				CCT_ASSERT_FALSE("ConcertoDotNet: could not load assembly");
			}
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
			[[unlikely]] if (functionPointer == nullptr)
				throw std::runtime_error("ConcertoDotNet: Could not find function " + functionName);
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
			if (functionPointer)
				functionPointer(std::forward<Args>(args)...);
		}

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
				if (functionPointer == nullptr)
					return nullptr;
				_loadedFunctions.emplace(functionName, reinterpret_cast<void*>(functionPointer));
				return functionPointer;
			}
			return static_cast<T*>(it->second);
		}

		CCT_OBJECT(Assembly);
	private:
		/**
		 * @brief Get the function pointer from the assembly
		 * @tparam T The function signature
		 * @param functionName The name of the function to get the pointer from
		 * @return The function pointer
		 */
		template<typename T>
		T* GetFunctionPointerFromAssembly(const std::string& functionName)
		{
			String dotnetType = _assemblyName + CCT_AUTO_WIDE_STRING(".Lib, ") + _assemblyName;
			T* functionPointer = nullptr;
			const std::filesystem::path path = std::filesystem::current_path() / _assemblyPath;

#ifdef CCT_PLATFORM_WINDOWS
			const auto wString = path.wstring();
			const char_t* str = wString.c_str();
			const String wFunctionName(functionName.begin(), functionName.end());

			const int rc = _getFunctionPointer(str, dotnetType.c_str(), wFunctionName.c_str(), nullptr, nullptr, reinterpret_cast<void**>(&functionPointer));
#else
			const auto string = path.string();
			const char_t* str = string.c_str();

			const int rc = _load_assembly_and_get_function_pointer(str, dotnetType.c_str(), functionName.c_str(), nullptr, nullptr, (void**)&functionPointer);
#endif

			if (rc != 0)
			{
				CCT_ASSERT_FALSE("ConcertoDotNet: Invalid return code {} -> {}", functionName, rc);
				return nullptr;
			}
			return functionPointer;
		}

		String _assemblyPath;
		String _assemblyName;
		std::unordered_map<std::string, void*> _loadedFunctions;
		FunctionRef<int(const char_t*, void*, void*)> _loadAssembly;
		std::function<int(const char_t*, const char_t*, const char_t*, void*, void*, void**)> _getFunctionPointer;
	};
} // Concerto
#endif //CONCERTO_DOTNET_ASSEMBLY_HPP
