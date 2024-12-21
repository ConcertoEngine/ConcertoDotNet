//
// Created by arthur on 07/03/2023.
//

#include <filesystem>
#include <gtest/gtest.h>
#include <Concerto/DotNet/HostFXR.hpp>

using namespace cct::dotnet;

//TEST(HostFXR, LoadHostFxr)
//{
//	try
//	{
//		std::string currentPath = std::filesystem::current_path().string();
//		HostFXR hostFxr(currentPath, "DotNetLib.runtimeconfig.json");
//	}
//	catch (const std::exception& e)
//	{
//		std::cerr << e.what() << std::endl;
//		FAIL();
//	}
//}
//
//TEST(HostFXR, LoadAssembly)
//{
//	try
//	{
//		std::string currentPath = std::filesystem::current_path().string();
//		HostFXR hostFxr(currentPath, "DotNetLib.runtimeconfig.json");
//		Assembly assembly = hostFxr.LoadDotNetAssembly("DotNetLib.dll", "DotNetLib");
//	}
//	catch (const std::exception& e)
//	{
//		std::cerr << e.what() << std::endl;
//		FAIL();
//	}
//}

TEST(HostFXR, InvokeAssembly)
{
	//try
	//{
	//	std::string currentPath = std::filesystem::current_path().string();
	//	HostFXR hostFxr(currentPath, "DotNetLib.runtimeconfig.json");
	//	Assembly assembly = hostFxr.LoadDotNetAssembly("DotNetLib.dll", "DotNetLib");
	//	struct lib_args
	//	{
	//		const char_t* message;
	//		int number;
	//	};
	//	lib_args args{
	//		.message = CCT_AUTO_WIDE_STRING("This string is passed from C++ to C#"),
	//		.number = 42
	//	};

	//	int x = assembly.Invoke<int>("Hello",(void*)&args, (int)sizeof(args));
	//	EXPECT_EQ(x, 0);

	//	x = assembly.Invoke<int>("Hello", (void*)&args, (int)sizeof(int)); // wrong size
	//	EXPECT_EQ(x, 1);
	//}
	//catch (const std::exception& e)
	//{
	//	std::cerr << e.what() << std::endl;
	//	FAIL();
	//}
}