#include "DotNetHello.hpp"

#include <thread>

#include "HostFXR.hpp"

#include <Concerto/Reflection/PackageLoader.hpp>
#include <Reflection/CorePackage.hpp>
#include "DotNetLibPackage.hpp"
#include "DotNetPackage.hpp"

using namespace std::string_view_literals;

int main()
{
	cct::refl::PackageLoader packageLoader;
	packageLoader.AddPackage(CreateCorePackage());
	packageLoader.AddPackage(CreateDotNetPackage());
	packageLoader.AddPackage(CreateDotNetLibPackage());

	packageLoader.LoadPackages();

	std::string currentPath = std::filesystem::current_path().string();
	cct::dotnet::HostFXR hostFxr(currentPath, "DotNetLib.runtimeconfig.json");

	const cct::refl::Class* klass = cct::dotnet::DotNetLibAssembly::GetClass();
	auto obj = klass->CreateDefaultObject<cct::dotnet::DotNetLibAssembly>();
	cct::Result<cct::Int32, std::string> res = hostFxr.LoadDotNetAssembly(*obj, "DotNetLib.dll", "DotNetLib");

	if (res.IsError())
	{
		cct::Logger::Error("HostFxr error '{}'", res.GetError());
		return EXIT_FAILURE;
	}

	//Print message in dotnet
	{
		const cct::refl::Method* method = klass->GetMethod("Hello"sv);
		static_cast<const cct::dotnet::DotNetMethod*>(method)->SetAssembly(*obj); //Fixme

		struct lib_args
		{
			const char_t* message;
			int number;
		};
		lib_args args{
			.message = CCT_AUTO_WIDE_STRING("This string is passed from C++ to C#"),
			.number = 42
		};
		cct::Result<cct::Int32, std::string> result = method->Invoke<cct::Int32>(*obj, static_cast<void*>(&args), static_cast<cct::UInt32>(sizeof(args)));
		result = method->Invoke<cct::Int32>(*obj, static_cast<void*>(&args), static_cast<cct::UInt32>(sizeof(args)));
	}

	//Dotnet edit vector of Int32
	{
		const cct::refl::Method* method = klass->GetMethod("UpdateIntVectorWithoutCopy"sv);
		static_cast<const cct::dotnet::DotNetMethod*>(method)->SetAssembly(*obj); //Fixme

		std::vector<cct::Int32> vec = { 1, 2, 3, 4, 5 };

		cct::Result<cct::Int32, std::string> result = method->Invoke<cct::Int32>(*obj, vec.data(), static_cast<cct::Int32>(vec.size()));

		cct::Logger::Info("Dotnet edited vector:\n");
		for (const auto& i : vec)
		{
			cct::Logger::Info("{}, ", i);
		}
	}
	return EXIT_SUCCESS;
}