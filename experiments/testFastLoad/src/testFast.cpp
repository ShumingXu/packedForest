#include "zipClassAndValue.h"
#include <vector>
#include <list>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <likwid.h>
#include <iomanip>
#include <chrono>



//////////Fast random number generator Start
#define znew (z=36969*(z&65535)+(z>>16))
#define wnew (w=18000*(w&65535)+(w>>16))
#define MWC ((znew<<16)+wnew )
#define SHR3 (jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5))
#define CONG (jcong=69069*jcong+1234567)
#define FIB ((b=a+b),(a=b-a))
#define KISS ((MWC^CONG)+SHR3)
#define LFIB4 (c++,t[c]=t[c]+t[UC(c+58)]+t[UC(c+119)]+t[UC(c+178)])
#define SWB (c++,bro=(x<y),t[c]=(x=t[UC(c+34)])-(y=t[UC(c+19)]+bro))
#define UNI (KISS*2.328306e-10)
#define VNI ((long) KISS)*4.656613e-10
#define UC (unsigned char) /*a cast operation*/
typedef unsigned long UL;
/* Global static variables: */
static UL z=362436069, w=521288629, jsr=123456789, jcong=380116160;
static UL a=224466889, b=7584631, t[256];
/* Use random seeds to reset z,w,jsr,jcong,a,b, and the table
 * t[256]*/
static UL x=0,y=0,bro; static unsigned char c=0;
/* Example procedure to set the table, using KISS: */
void settable(UL i1,UL i2,UL i3,UL i4,UL i5, UL i6)
{ int i; z=i1;w=i2,jsr=i3; jcong=i4; a=i5; b=i6;
	for(i=0;i<256;i=i+1) t[i]=KISS;
}
//////////Fast random number generator Stop

class iterBeginEnd{
	private:
		int start;
		int end;

	public:
		inline void setStart(int startSetter){
			start = startSetter;
		}

		inline void setEnd(int endSetter){
			end = endSetter;
		}

		inline void setBoth(int startSetter, int endSetter){
			start = startSetter;
			end = endSetter;
		}

		inline int returnStart(){
			return start;
		}

		inline void incrementStart(){
			++start;
		}

		inline bool returnAtEnd(){
			return start==end;
		}
};


class roundRobinLoad
{
	protected:
		std::list<iterBeginEnd> iteratorList;

	public:
		roundRobinLoad(): iteratorList(15){}

		inline void createIterators(int indexVecSize){
			int binSize = indexVecSize/15;
			int binStart = 0;
			int binEnd = binStart+binSize;
			auto iter = iteratorList.begin();
			for(; iter != iteratorList.end(); ++iter){
				iter->setBoth(binStart, binEnd);
				binStart = binEnd;
				binEnd = binStart +binSize;
			}
			iteratorList.emplace_back();
			iteratorList.back().setBoth(binStart, indexVecSize);
		}

		inline std::list<iterBeginEnd>& returnList(){
			return iteratorList;
		}

		inline bool isEmpty(){
			return iteratorList.empty();
		}

		inline std::list<iterBeginEnd>::iterator returnFront(){
			return iteratorList.begin();
		}

		inline std::list<iterBeginEnd>::iterator returnEnd(){
			return iteratorList.end();
		}

		inline std::list<iterBeginEnd>::iterator eraseIt(std::list<iterBeginEnd>::iterator dIt){
			return iteratorList.erase(dIt);
		}
};


int main(int argc, char* argv[]) {

	if (argc != 3){
		std::cout << argv[0] << " requires two arguments\n";	
		return -1;
	}
	int alg = atoi(argv[1]);
	int testSize = atoi(argv[2]);
	int searchSize = 10*testSize;


	std::vector<fp::zipClassAndValue<int, UL> > zipVec;
	std::vector<UL>  randomValVec;
	std::vector<int> randomOrderVec;

	zipVec.resize(testSize);

	randomOrderVec.resize(testSize);
	for(int i = 0;i < testSize;++i){
		randomOrderVec[i]=(int)(MWC%(searchSize));
	}

	randomValVec.resize(searchSize);
	for(int i = 0;i < searchSize;++i){
		randomValVec[i]=(UL)i;
	}
	/////Clear Cache
	std::vector<std::vector<int> > cacheClearVec(10000);
	for(int i = 0;i < 10000;++i){
		cacheClearVec[i].resize(100000);
		for(int j = 0; j < 100000; ++j){
			cacheClearVec[i][j]=(int)MWC;
		}
	}
	//////////

	/////////////////////////////////
	//Start Timer
	////////////////////////////////
	struct timespec start, finish;
	double elapsed;

	clock_gettime(CLOCK_MONOTONIC, &start);


	LIKWID_MARKER_INIT;
	LIKWID_MARKER_START("loadNode");
	///////Run test code
	if(alg==1){
		for(int i = 0; i < testSize; ++i){
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
		}
	}else if(alg==2){
		for(int i = 0; i < testSize; ++i){
			__builtin_prefetch (&randomValVec[randomOrderVec[i]], 0, 0);
			__builtin_prefetch (&zipVec[i], 1, 0);
		}
		for(int i = 0; i < testSize; ++i){
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
		}
	}else if(alg==3){
		for(int i = 0; i < testSize; ++i){
			__builtin_prefetch (&randomValVec[randomOrderVec[i]], 0, 3);
		}
		for(int i = 0; i < testSize; ++i){
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
		}
	}else if(alg==4){
		roundRobinLoad rrIters;
		int loadPosition = 0;

		rrIters.createIterators((int)randomOrderVec.size());
		for(auto i : rrIters.returnList()){
			__builtin_prefetch (&randomValVec[randomOrderVec[i.returnStart()]], 0, 3);
		}

		while(!rrIters.isEmpty()){
			for(auto iter = rrIters.returnFront(); iter != rrIters.returnEnd();){
				if(iter->returnAtEnd()){
					iter = rrIters.eraseIt(iter);
					continue;
				}
				__builtin_prefetch (&randomValVec[randomOrderVec[iter->returnStart()+1]], 0, 3);
				zipVec[loadPosition].setPair(randomOrderVec[iter->returnStart()], randomValVec[randomOrderVec[iter->returnStart()]]);
				++loadPosition;
				iter->incrementStart();
				++iter;
			}
		}

	}else if(alg==5){
		int numBins = 32;
		std::vector<int> position(numBins);
		int binSize = (int)randomOrderVec.size() / numBins;
		int currPos = 0;


		for(auto &i : position){
			i = currPos;
			__builtin_prefetch (&randomValVec[randomOrderVec[i]], 0, 3);
			currPos += binSize;
		}

		int currI;
		for(int j=0; j < binSize;++j){
			for(auto& i : position){
				currI = i++;
				__builtin_prefetch (&randomValVec[randomOrderVec[i]], 0, 3);
				zipVec[currI].setPair(randomOrderVec[currI], randomValVec[randomOrderVec[currI]]);
			}
		}
		for(int i = currPos; i < (int)randomOrderVec.size(); ++i){
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
		}
	}else if(alg==6){	
		for(int i = 0; i < testSize;){
			__builtin_prefetch (&randomValVec[i], 0, 3);
			i+=i+16;
		}
		for(int i = 0; i < testSize; ++i){
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
		}
	}else if(alg==7){	

			__builtin_prefetch (&randomValVec[0], 0, 3);
			__builtin_prefetch (&randomValVec[1], 0, 3);
			__builtin_prefetch (&randomValVec[2], 0, 3);
			__builtin_prefetch (&randomValVec[3], 0, 3);
			__builtin_prefetch (&randomValVec[4], 0, 3);
			__builtin_prefetch (&randomValVec[5], 0, 3);
			__builtin_prefetch (&randomValVec[6], 0, 3);
			__builtin_prefetch (&randomValVec[7], 0, 3);

		for(int i = 0; i < testSize- testSize%8;){
			__builtin_prefetch (&randomValVec[i+8], 0, 3);
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
			++i;
			__builtin_prefetch (&randomValVec[i+8], 0, 3);
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
			++i;
			__builtin_prefetch (&randomValVec[i+8], 0, 3);
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
			++i;
			__builtin_prefetch (&randomValVec[i+8], 0, 3);
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
			++i;
			__builtin_prefetch (&randomValVec[i+8], 0, 3);
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
			++i;
			__builtin_prefetch (&randomValVec[i+8], 0, 3);
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
			++i;
			__builtin_prefetch (&randomValVec[i+8], 0, 3);
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
			++i;
			__builtin_prefetch (&randomValVec[i+8], 0, 3);
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
			++i;
		}
		for(int i = testSize- testSize%8; i < testSize; ++i){
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
		}
}else if(alg==8){
	int numAtTime = 1000;
		for(int i = 0; i < numAtTime; ++i){
			__builtin_prefetch (&randomValVec[randomOrderVec[i]], 0, 0);
			__builtin_prefetch (&zipVec[i], 1, 0);
		}
		for(int i = 0; i < testSize-numAtTime; ++i){
__builtin_prefetch (&randomValVec[randomOrderVec[i+numAtTime]], 0, 0);
			__builtin_prefetch (&zipVec[i+numAtTime], 1, 0);
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
		}
for(int i = testSize-numAtTime; i < testSize; ++i){
			zipVec[i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
		}
	}else{	

		std::cout << "unknown alg selected" << std::endl;
		for(int i = 0; i < 100; ++i){
			std::cout << randomOrderVec[i] << "\n";
		}
		return -1;
	}

	LIKWID_MARKER_STOP("loadNode");
	LIKWID_MARKER_CLOSE;
	/////////////////////////////////
	//Stop Timer
	////////////////////////////////

	clock_gettime(CLOCK_MONOTONIC, &finish);

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

	std::cout << "It ran in " << elapsed << "s\n";
	std::cout << "last random " << cacheClearVec[999][99999] << "\n";

	//////////////////////////
	//Correctness Test
	//////////////////////////
	std::vector<std::vector<fp::zipClassAndValue<int, UL> > > zipVecCheck;
	zipVec.resize(testSize);





}
