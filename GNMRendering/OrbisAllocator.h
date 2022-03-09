#pragma once

#include <gnm.h>
#include <gnmx\context.h>

namespace sce {
	namespace Gnmx {
		namespace Toolkit {
			struct IAllocator;
		}
	}
}

namespace NCL {
	namespace PS4 {
		class OrbisAllocator {
		protected:
			OrbisAllocator() {}
			~OrbisAllocator() {}
		protected:
			static sce::Gnmx::Toolkit::IAllocator* onionAllocator;
			static sce::Gnmx::Toolkit::IAllocator* garlicAllocator;
			static sce::Gnm::OwnerHandle			ownerHandle;
		};
	}
}

