//
// Created by arthur on 20/12/2024.
//

#include "Concerto/DotNet/DotNetMethod.hpp"
#include "Concerto/DotNet/Assembly.hpp"

namespace cct::dotnet
{
	DotNetMethod::DotNetMethod(std::string_view name, const cct::refl::Class* returnValue, std::vector<const cct::refl::Class*> parameters, std::size_t index) :
		Method(name, returnValue, std::move(parameters), index),
		_assembly(nullptr)
	{
	}

	bool DotNetMethod::SetAssembly(Assembly& assembly)
	{
		_assembly = &assembly;

		auto* func = assembly.GetFunctionPointer<void()>(std::string(GetName()));
		if (func == nullptr)
			return false;
		SetCustomInvoker(reinterpret_cast<void*>(func));
		return true;
	}
}
