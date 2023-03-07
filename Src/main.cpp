#include <iostream>
#include <chrono>

#include "HostFXR.hpp"

int main()
{
	// get the current time
	auto start = std::chrono::high_resolution_clock::now();
	std::string currentPath = std::filesystem::current_path().string();
	Concerto::DotNet::HostFXR hostfxr(currentPath, "DotNetLib.runtimeconfig.json");
	Concerto::DotNet::Assembly assembly = hostfxr.LoadDotNetAssembly("DotNetLib.dll", "DotNetLib");
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time taken for initialization: "
			  << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
			  << " microseconds" << std::endl;
	struct lib_args
	{
		const char_t* message;
		int number;
	};
	std::wstring message = L"This string is passed from C++ to C#";
	lib_args args{
		.message =message.c_str(),
		.number = 42
	};
	start = std::chrono::high_resolution_clock::now();
	int x = assembly.Invoke<int>("Hello",(void*)&args, (int)sizeof(args));
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Time taken for execution: "
			  << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
			  << " microseconds" << std::endl;
}