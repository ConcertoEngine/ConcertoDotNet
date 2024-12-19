#include "DotNetHello.hpp"
#include <Reflection/CorePackage.hpp>
#include <array>
#include "../build/.gens/concerto-dotnet-sample/windows/x64/release/Reflection/DotNetPackage.hpp"


namespace cct
{
	void DotNetHello::Hello(const std::string& text)
	{
	}
}

void LoadPackages(std::span<std::unique_ptr<cct::refl::Package>> packages)
{
	for (const auto& pkg : packages)
		pkg->LoadNamespaces();
	for (const auto& pkg : packages)
		pkg->InitializeNamespaces();
	for (const auto& pkg : packages)
		pkg->InitializeClasses();
}
int main()
{
	std::array packages = { CreateCorePackage(), CreateCorePackage() };
	LoadPackages(packages);
	return 0;
}