#pragma once

namespace tue
{
	inline namespace constraints
	{
		struct non_transferable
		{
			non_transferable() = default;
			non_transferable(non_transferable const&) = delete;
			non_transferable(non_transferable     &&) = delete;
			non_transferable & operator = (non_transferable const&) = delete;
			non_transferable & operator = (non_transferable     &&) = delete;
		};
	};
}

