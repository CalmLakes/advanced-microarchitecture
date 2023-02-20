
#include <circular_fifo.h>

circular_fifo::circular_fifo (uint32_t fifoSize){
    FIFO_LENGTH = fifoSize;
    list = new T[fifoSize];
    head = tail = 0;
    tail_phase = head_phase = false;
}

void circular_fifo::push(T item){
    list[tail] = reg;
    tail++;
    if (tail == FIFO_LENGTH){
        tail = 0;
        tail_phase != tail_phase;
    }
}

T circular_fifo::pop(T item){
    item = list[head];
    head++;
    if (head == FIFO_LENGTH){
        head = 0;
        head_phase != head_phase;
    }
    return item;
}