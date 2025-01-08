//
// Created by arthur on 20/12/2024.
//

#ifndef CONCERTO_DOTNET_DOTNETMETHOD_HPP
#define CONCERTO_DOTNET_DOTNETMETHOD_HPP

#include <Concerto/Core/Result.hpp>

#include <Concerto/Reflection/Class.hpp>
#include <Concerto/Reflection/Method.hpp>
#include <Concerto/Reflection/MemberVariable.hpp>
#include <Concerto/Reflection/Namespace.hpp>

#include "Assembly.hpp"
#include "Concerto/DotNet/Defines.hpp"

namespace cct::dotnet
{
	class CCT_DOTNET_API DotNetMethod : public cct::refl::Method
	{
	public:
		DotNetMethod(std::string_view name, const cct::refl::Class* returnValue, std::vector<const cct::refl::Class*> parameters, std::size_t index);

		bool SetAssembly(Assembly& assembly) const;
	private:
		mutable Assembly* _assembly;
	};
}

#endif //CONCERTO_DOTNET_DOTNETMETHOD_HPP