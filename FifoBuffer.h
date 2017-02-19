#ifndef FifoBuffer_h
#define FifoBuffer_h

#define FIFO_BUFFER_SIZE 36

class FifoBuffer {
  private:
    byte _data[FIFO_BUFFER_SIZE];
    byte _first = 0;
  public:  
    byte count = 0;
    bool isFull() { return (count == FIFO_BUFFER_SIZE); }
    bool push(byte value);
    bool peek(byte *value);
    bool pop(byte *value); 
    void clear();   
    void dump();
};

bool FifoBuffer::push(byte value) {
  if (count < FIFO_BUFFER_SIZE) {
    count++;
    byte i = _first + count - 1;
    if (i >= FIFO_BUFFER_SIZE) i -= FIFO_BUFFER_SIZE;
    _data[i] = value;
    return true;
  } else {
    return false;
  }
}

bool FifoBuffer::peek(byte *value) {
  if (count > 0) {
    *value = _data[_first];
    return true;
  } else {
    return false;
  }  
}

bool FifoBuffer::pop(byte *value) {
  if (count > 0) {
    *value = _data[_first];
    _first++;
    if (_first == FIFO_BUFFER_SIZE) _first = 0;
    count--;
    return true;
  } else {
    return false;
  }
}

void FifoBuffer::clear() {
  _first = 0;
  count = 0;
}

void FifoBuffer::dump() {
  byte b,i,c;
  c = count;
  for (i=0;i<c;i++) {
    pop(&b);
    push(b);
    Serial.print(b,HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

#endif
