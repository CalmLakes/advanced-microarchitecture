#include <free_list.h>

free_list_def::free_list_def (uint64_t fifoSize){
    FIFO_LENGTH = fifoSize;
    list = new uint64_t[fifoSize];
    fifo_size = 0;
    head = tail = 0;
    tail_phase = head_phase = false;
};

void free_list_def::push(uint64_t item){
    list[tail] = item;
    tail++;
    fifo_size++;
    if (tail == FIFO_LENGTH){
        tail = 0;
        tail_phase != tail_phase;
    }
};

uint64_t free_list_def::pop(){
    item = list[head];
    head++;
    fifo_size--;
    if (head == FIFO_LENGTH){
        head = 0;
        head_phase != head_phase;
    }
    return item;
};

free_list_def::~free_list_def(){
    delete [] list;
};

uint64_t free_list_def::size(){
    return fifo_size;
};

// Returns item at a given index. Correctly indexes the circular fifo
uint64_t free_list_def::at(uint64_t index){
    if (head + index > FIFO_LENGTH) return list[head + index - FIFO_LENGTH];
    else return list[head + index];
};

void free_list_def::setTail(uint64_t value){
    if (value > FIFO_LENGTH){
        tail = value = FIFO_LENGTH;
    }
    else {
        tail = value;
    }
};

bool free_list_def::empty(){
    return (fifo_size == 0);
};

void free_list_def::flush(){
    fifo_size = 0;
    head = tail = 0;
    tail_phase = head_phase = false;
};

