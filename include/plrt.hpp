/****************************************\
 pl32lib-ng, v1.06
 (c) 2022 pocketlinux32, Under MPL v2.0
 plrt.hpp: Base API header (C++ Bindings)
\****************************************/

#include <cstddef>
#include <cstdint>
#include <cstdio>

#define PLRTCPP

namespace plRT {
	namespace cApi {
		extern "C"{
			#include <plrt.h>
		}
	}

	namespace memory {
		class tracker {
			private:
				plRT::cApi::plmt_t* mt;
			public:
				tracker(size_t maxMemoryAmnt){
					mt = plRT::cApi::plMTInit(maxMemoryAmnt);
				}

				tracker(){
					mt = NULL;
				}

				~tracker(){
					if(mt != NULL)
						plRT::cApi::plMTStop(mt);
				}

				void init(size_t maxMemoryAmnt){
					if(mt == NULL)
						mt = plRT::cApi::plMTInit(maxMemoryAmnt);
				}

				size_t getUsedSize(){
					return plRT::cApi::plMTMemAmnt(mt, plRT::cApi::PLMT_GET_USEDMEM, 0);
				}

				size_t getMaxSize(){
					return plRT::cApi::plMTMemAmnt(mt, plRT::cApi::PLMT_GET_MAXMEM, 0);
				}

				void setMaxSize(size_t newMaxSize){
					plRT::cApi::plMTMemAmnt(mt, plRT::cApi::PLMT_SET_MAXMEM, newMaxSize);
				}

				plRT::cApi::memptr_t alloc(size_t size){
					return plRT::cApi::plMTAlloc(mt, size);
				}

				plRT::cApi::memptr_t calloc(size_t size, size_t amount){
					return plRT::cApi::plMTCalloc(mt, size, amount);
				}

				plRT::cApi::memptr_t realloc(plRT::cApi::memptr_t pointer, size_t newSize){
					return plRT::cApi::plMTRealloc(mt, pointer, newSize);
				}

				void free(plRT::cApi::memptr_t pointer){
					plRT::cApi::plMTFree(mt, pointer);
				}

				plRT::cApi::plmt_t* getMTHandle(){
					return mt;
				}
		};

		class fatPointer {
			private:
				plRT::cApi::plptr_t fatPtr;
				tracker &tracker;
			public:
				fatPointer(plRT::cApi::memptr_t pointer, size_t size, tracker &tracker, bool is2dimArray){
					fatPtr.pointer = pointer;
					fatPtr.size = size;
					fatPtr.isMemAlloc = isMemAlloc;
					fatPtr.mt = tracker.getMTHandle();
					is2DArray = is2dimArray;
				}

				fatPointer(plRT::cApi::memptr_t pointer, size_t size, bool is2dimArray){
					fatPtr.pointer = pointer;
					fatPtr.size = size;
					fatPtr.isMemAlloc = false;
					fatPtr.mt = NULL;
					is2DArray = is2dimArray;
				}

				~fatPointer(){
					if(fatPtr.isMemAlloc && fatPtr.mt != NULL)
						plRT::cApi::plMTFreeArray(&fatPtr, is2DArray);
				}

				plRT::cApi::memptr_t getRawPointer(){
					return fatPtr.pointer;
				}
		};
	}

	std::string plStoCppString(plRT::cApi::plstring_t string){
		
	}

	namespace parser {
		memory::fatPointer parse(plRT::cApi::plstring_t input, memory::tracker &tracker){
			plRT::cApi::plfatptr_t* tempData = plRT::cApi::plParser(input, tracker.getMTHandle());

			memory::fatPointer returnPointer(tempData->array, tempData->size, true, true, tracker);
			tracker.free(tempData);

			return returnPointer;
		}
	}

	class file {
		private:
			memory::tracker tracker;
			plRT::cApi::plfile_t* fileHandle;
		public:
			file(plRT::cApi::string_t filename, plRT::cApi::string_t mode){
				tracker.init(1024 * 1024);
				fileHandle = plRT::cApi::plFOpen(filename, mode, tracker.getMTHandle());
			}

			file(size_t sizeOfBuffer){
				tracker.init(sizeOfBuffer);
				fileHandle = plRT::cApi::plFOpen(NULL, NULL, tracker.getMTHandle());
			}

			~file(){
				plRT::cApi::plFClose(fileHandle);
			}

			memory::fatPointer read(size_t amountOfBytes, memory::tracker &extTracker){
				plRT::cApi::memptr_t tempPtr = extTracker.alloc(amountOfBytes);
				plRT::cApi::plFRead(tempPtr, amountOfBytes, 1, fileHandle);

				return memory::fatPointer(tempPtr, amountOfBytes, false, true, extTracker);
			}

			void write(memory::fatPointer data){
				plRT::cApi::plFWrite(data.getPointer(), data.getSize(), 1, fileHandle);
			}
	};

	
}
