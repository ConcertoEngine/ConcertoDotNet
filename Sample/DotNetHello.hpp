#pragma once
#include <string>
#include <Concerto/Reflection/Object.hpp>


namespace cct
{
	class DotNetHello : public cct::refl::Object
	{
	public:
		void Hello(const std::string& text);
		CCT_OBJECT(DotNetHello);
	};
}
