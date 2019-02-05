#ifndef fpForestBase_h
#define fpForestBase_h

#include "displayProgress.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>

namespace fp{

	template <typename T>
		class fpForestBase
		{

			public:

				fpForestBase(){
				std::srand(unsigned(std::time(0)));
				}

				virtual ~fpForestBase() {}
				virtual void printForestType() = 0;
				virtual void growForest() = 0;
				virtual float testForest() = 0;
				virtual int predictClass(std::vector<T>& observation) = 0;
				virtual int predictClass(const T* observation) = 0;
		};

}//namespace fp
#endif //fpForestBase.h
