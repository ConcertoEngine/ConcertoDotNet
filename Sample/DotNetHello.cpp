#include "DotNetHello.hpp"

#include <thread>

#include "HostFXR.hpp"

namespace cct
{
	void DotNetHello::Hello(const std::string& text)
	{
	}
}

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

	const cct::refl::Class* klass = cct::dotnet::DotNetLib::GetClass();
	auto obj = klass->CreateDefaultObject<cct::dotnet::DotNetLib>();
	cct::Result<cct::Int32, std::string> res = hostFxr.LoadDotNetAssembly(*obj, "E:/Documents/git/ConcertoEngine/ConcertoDotNet/build/windows/x64/debug/obj/DotNetLib/debug/DotNetLib.dll", "DotNetLib");

	if (res.IsError())
	{
		cct::Logger::Error("HostFxr error '{}'", res.GetError());
		return EXIT_FAILURE;
	}

	const cct::refl::Method* method = klass->GetMethod("Hello"sv);
	const_cast<cct::dotnet::DotNetMethod*>(static_cast<const cct::dotnet::DotNetMethod*>(method))->SetAssembly(*obj);
	struct lib_args
	{
		const char_t* message;
		int number;
	};
	lib_args args{
		.message = CCT_AUTO_WIDE_STRING("This string is passed from C++ to C#"),
		.number = 42
	};
	cct::Result<cct::Int32, std::string> result = method->Invoke<cct::Int32>(*obj, (void*) &args, static_cast<cct::UInt32>(sizeof(args)));
	return EXIT_SUCCESS;
}