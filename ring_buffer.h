#pragma once

template <class T>
class RingBuffer {
private:
  T* buffer;
  uint64_t write_ptr = 0;
  uint64_t read_ptr = 0;
  uint64_t capacity;

public:
  RingBuffer(uint64_t capacity) {
      buffer = new T[capacity];
      this->capacity = capacity;
  }

  void push(T& val) {
    buffer[++write_ptr % capacity] = val;
  }

  T& pop() {
    return buffer[read_ptr++ % capacity];
  }

  bool empty() {
    return write_ptr < read_ptr;
  }

  bool full() {
    return (write_ptr - read_ptr + 1) == capacity;
  }

  void reset() {
    read_ptr = 0;
    write_ptr = 0;
  }

  void setReadPtr(uint64_t ptr) {
    read_ptr = ptr;
  }

  void setWritePtr(uint64_t ptr) {
    write_ptr = ptr;
  }

  uint64_t getWritePtr() {
    return write_ptr;
  }
};