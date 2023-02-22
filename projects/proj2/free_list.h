#include <inttypes.h>

class free_list_def {
		private: 
			uint64_t FIFO_LENGTH;
			uint64_t item;
            uint64_t fifo_size;
		public:
            uint64_t tail;
            uint64_t head;
            bool tail_phase;
			bool head_phase;
            uint64_t * list;
			free_list_def(uint64_t fifoSize);
			void push(uint64_t item);
			uint64_t pop();
			~free_list_def();
            uint64_t size();
            uint64_t at(uint64_t index);
            void setTail(uint64_t value);
            bool empty();
            void flush();
	};