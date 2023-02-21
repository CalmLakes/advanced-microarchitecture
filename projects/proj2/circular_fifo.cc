#include <circular_fifo.h>

template <class T> circular_fifo<T>::circular_fifo (uint64_t fifoSize){
    FIFO_LENGTH = fifoSize;
    list = new T[fifoSize];
    fifo_size = 0;
    head = tail = 0;
    tail_phase = head_phase = false;
};

template <class T> void circular_fifo<T>::push(T item){
    list[tail] = item;
    tail++;
    fifo_size++;
    if (tail == FIFO_LENGTH){
        tail = 0;
        tail_phase != tail_phase;
    }
};

template <class T> T circular_fifo<T>::pop(){
    item = list[head];
    head++;
    fifo_size--;
    if (head == FIFO_LENGTH){
        head = 0;
        head_phase != head_phase;
    }
    return item;
};

template <class T> circular_fifo<T>::~circular_fifo(){
    delete [] list;
};

template <class T> uint64_t circular_fifo<T>::size(){
    return fifo_size;
};

// Returns item at a given index. Correctly indexes the circular fifo
template <class T> T circular_fifo<T>::at(uint64_t index){
    if (head + index > FIFO_LENGTH) return list[head + index - FIFO_LENGTH];
    else return list[head + index];
};

template <class T> void circular_fifo<T>::setTail(uint64_t value){
    if (value > FIFO_LENGTH){
        tail = value = FIFO_LENGTH;
    }
    else {
        tail = value;
    }
};

template <class T> bool circular_fifo<T>::empty(){
    return (fifo_size == 0);
};

template <class T> void circular_fifo<T>::flush(){
    fifo_size = 0;
    head = tail = 0;
    tail_phase = head_phase = false;
};

