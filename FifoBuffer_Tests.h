#include "Test_Framework.h"

FifoBuffer buf;

bool fillFifoTest() {
  byte i;
  for (i=0;i<FIFO_BUFFER_SIZE;i++) { 
    if (buf.push(i)) {
      if (buf.count != i+1) {
        return false;
      }  
    } else {
      return false;    
    }
  }
  return true;
}

bool unfillFifoTest() {
  byte b,i;
  for (i=0;i<FIFO_BUFFER_SIZE;i++) { 
    if (buf.pop(&b)) {
      if (b != i) {
        return false;
      } else {
        if (buf.count != (FIFO_BUFFER_SIZE - i - 1)) {
          return false;
        }
      }
    } else {
      return false;  
    }
  }
  return (buf.count == 0);  
}

bool fifoOverflowTest() {
  byte i;
  for (i=0;i<FIFO_BUFFER_SIZE;i++) {
    if (buf.isFull())
      return false; 
    buf.push(i);
  }  

  if (!buf.isFull()) 
    return false;
  return !buf.push(99);
}

bool fifoClearTest() {
  if (buf.isFull()) {
    buf.clear();
    return (buf.count == 0);
  } else {
    return false;
  }
}

void runFifoTests() {
  byte b;
  printTestTitle("FIFO Buffer");
  printTestResult("Init Count to Zero",buf.count==0);
  printTestResult("Push Number #1",(buf.push(1) && (buf.count == 1))); 
  printTestResult("Pop Number #1",(buf.pop(&b) && (b == 1) && (buf.count == 0)));
  printTestResult("Push Number #2",(buf.push(2) && (buf.count == 1))); 
  printTestResult("Pop Number #2",(buf.pop(&b) && (b == 2) && (buf.count == 0)));
  printTestResult("Push Number #3",(buf.push(3) && (buf.count == 1) && buf.push(4) && (buf.count == 2))); 
  printTestResult("Pop Number #3a",(buf.pop(&b) && (b == 3) && (buf.count == 1)));
  printTestResult("Pop Number #3b",(buf.pop(&b) && (b == 4) && (buf.count == 0)));
  printTestResult("Fill Fifo Test",fillFifoTest());
  printTestResult("Unfill Fifo Test",unfillFifoTest());
  printTestResult("Fifo Overflow Test",fifoOverflowTest());
  printTestResult("Clear Test",fifoClearTest());
  printTestResult("Underflow Test",((buf.count==0) && (!buf.pop(&b)) && (buf.count==0)));
}

