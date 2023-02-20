template <typename T> class circular_fifo {
		private: 
			uint32_t FIFO_LENGTH;
			uint32_t head;
			uint32_t tail;
			bool tail_phase;
			bool head_phase;
			T item;
			T list[];
		public:
			circular_fifo (uint32_t fifoSize);
			void push(T item);
			T pop(T item);
	}