#include <active_list.h>
#include <bits/stdc++.h>

active_list::active_list(uint64_t fifoSize){
    FIFO_LENGTH = fifoSize;
    list = new active_list_entry * [fifoSize];
    fifo_size = 0;
    head = tail = 0;
    tail_phase = head_phase = false;
}

void active_list::push(active_list_entry * item){
    list[tail] = item;
    tail++;
    if (tail == FIFO_LENGTH){
        tail = 0;
        tail_phase = !tail_phase;
    }
}
// Test bro
active_list_entry * active_list::pop(){
    item = list[head];
    head++;
    if (head == FIFO_LENGTH){
        head = 0;
        head_phase = !head_phase;
    }
    return item;
}

active_list::~active_list(){
    delete [] list;
}

uint64_t active_list::size(){
    if (tail < head){
        return (FIFO_LENGTH - head + tail);
    }
    else if (tail == head){
        if (tail_phase == head_phase) return 0;
        else return FIFO_LENGTH;
    }
    else return (tail - head);
}

// Returns item at a given index. Correctly indexes the circular fifo
active_list_entry * active_list::at(uint64_t index){
    printf("Active list size: %d\n",FIFO_LENGTH);
    printf("Head: %d | Tail: %d\n",head,tail);
    if (head + index >= FIFO_LENGTH) return list[head + index - FIFO_LENGTH];
    else return list[head + index];
}

void active_list::setTail(uint64_t value){
    if (value >= FIFO_LENGTH){
        tail = value - FIFO_LENGTH;
        tail_phase = !tail_phase;
    }
    else {
        tail = value;
    }
}

bool active_list::empty(){
    return (head == tail) && (tail_phase == head_phase);
}

void active_list::flush(){
    head = tail = 0;
    tail_phase = head_phase = false;
}

bool active_list::full(){
    return (head == tail) && (tail_phase != head_phase);
}

