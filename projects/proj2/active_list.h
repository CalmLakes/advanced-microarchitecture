#include <inttypes.h>

struct active_list_entry{
		bool dest_flag;
		uint64_t logical_reg_num;
		uint64_t physical_reg_num;
		bool completed;
		bool exception;
		bool load_violation;
		bool branch_mispred;
		bool val_mispred;
		bool load_flag;
		bool store_flag;
		bool branch_flag;
		bool amo_flag;
		bool csr_flag;
		uint64_t pc;

		active_list_entry(bool dest_valid,
                        uint64_t log_reg,
                        uint64_t phys_reg,
                        bool load,
                        bool store,
                        bool branch,
                        bool amo,
                        bool csr,
                        uint64_t PC){

		this->dest_flag = dest_valid;
		this->logical_reg_num = log_reg;
		this->physical_reg_num = phys_reg;
		this->load_flag = load;
		this->store_flag = store;
		this->branch_flag = branch;
		this->amo_flag = amo;
		this->csr_flag = csr;
		this->pc = PC;
		}
        active_list_entry(){}
	};

class active_list_def {
		private: 
			uint64_t FIFO_LENGTH;
			active_list_entry item;
            uint64_t fifo_size;
		public:
            uint64_t tail;
            uint64_t head;
            bool tail_phase;
			bool head_phase;
            active_list_entry * list;
			active_list_def(uint64_t fifoSize);
			void push(active_list_entry item);
			active_list_entry pop();
			~active_list_def();
            uint64_t size();
            active_list_entry at(uint64_t index);
            void setTail(uint64_t value);
            bool empty();
            void flush();
	};