#include <free_list.h>
#include <bits/stdc++.h>
free_list::free_list (uint64_t fifoSize){
    FIFO_LENGTH = fifoSize;
    list = new uint64_t[fifoSize];
    fifo_size = 0;
    head = tail = 0;
    tail_phase = head_phase = false;
};

void free_list::push(uint64_t item){
    list[tail] = item;
    tail++;
    fifo_size++;
    if (tail == FIFO_LENGTH){
        tail = 0;
        tail_phase = !tail_phase;
    }
};

uint64_t free_list::pop(){
    item = list[head];
    head++;
    fifo_size--;
    if (head == FIFO_LENGTH){
        head = 0;
        head_phase = !head_phase;
    }
    return item;
};

free_list::~free_list(){
    delete [] list;
};

uint64_t free_list::size(){
    if (tail < head){
        return (FIFO_LENGTH - head + tail);
    }
    else if (tail == head){
        if (tail_phase == head_phase){
            printf("Free List is Empty\n");
            return 0;
        } 
        else return FIFO_LENGTH;
    }
    else return (tail - head);
};

// Returns item at a given index. Correctly indexes the circular fifo
uint64_t free_list::at(uint64_t index){
    if (head + index > FIFO_LENGTH) return list[head + index - FIFO_LENGTH];
    else return list[head + index];
};


bool free_list::empty(){
    return (fifo_size == 0);
};

void free_list::flush(){
    printf("Flushing\n");
    head = tail;
    head_phase = tail_phase = 0;
}

