#ifndef fpForestBase_h
#define fpForestBase_h

#include "../baseFunctions/fpUtils.h"
namespace fp{

class fpForestBase
{

	public:
		virtual void printForestType() = 0;
		virtual void growForest(fpInfo& info, fpData& dat) = 0;
};

}//namespace fp
#endif //fpForestBase.h