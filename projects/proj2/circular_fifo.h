#include <inttypes.h>
template <class T> class circular_fifo {
		private: 
			uint32_t FIFO_LENGTH;
			uint32_t tail;
			T item;
			T * list;
            uint64_t fifo_size;
		public:
            uint32_t head;
            bool tail_phase;
			bool head_phase;
			circular_fifo (uint64_t fifoSize);
			void push(T item);
			T pop();
			~circular_fifo();
            uint64_t size();
	};