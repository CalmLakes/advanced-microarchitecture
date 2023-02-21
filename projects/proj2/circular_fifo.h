#include <inttypes.h>
template <class T> class circular_fifo {
		private: 
			uint64_t FIFO_LENGTH;
			T item;
            uint64_t fifo_size;
		public:
            uint64_t tail;
            uint64_t head;
            bool tail_phase;
			bool head_phase;
            T * list;
			circular_fifo (uint64_t fifoSize);
			void push(T item);
			T pop();
			~circular_fifo();
            uint64_t size();
            T at(uint64_t index);
            void setTail(uint64_t value);
            bool empty();
            void flush();
	};