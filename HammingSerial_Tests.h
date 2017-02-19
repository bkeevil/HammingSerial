#include "Test_Framework.h"

HammingSerial master(&Serial1);
HammingSerial slave(&Serial2);

long throughput;

bool sendByteTest() {
  byte b,x;
  b = 3;
  if (master.tx_buffer.push(b)) {
    if (master.send()) {
      Serial1.flush();
      if (slave.receive()) {
        if (slave.rx_buffer.pop(&x)) {
          if (x == b) {
            return true;
          }
        }
      }
    }
  }  
  return false;
}

bool send16ByteTest() {
  byte i,b;
  for (i=0;i<16;i++) {
    if (!master.tx_buffer.push(i*7)) return false;
  }

    if (master.send()) {
      Serial1.flush();
      if (slave.receive()) {
        for (i=0;i<16;i++) {
          if (slave.rx_buffer.pop(&b)) {
            if (i*7 != b) 
              return false;
          } else return false;
        }
      } else return false;
    } else return false;
    
  return true;
}

bool randomPacketTest0() {
  byte i,x;
  byte b[16];
  
  for (i=0;i<16;i++) {
    b[i] = random(256);
    if (!master.tx_buffer.push(b[i])) 
      return false;
  }

    if (master.send()) {
      Serial1.flush();
      if (slave.receive()) {
        for (i=0;i<16;i++) {
          if (slave.rx_buffer.pop(&x)) {
            if (b[i] != x) 
              return false;
          } else return false;
        }
      } else return false;
    } else return false;
    
  return true;
}  

bool randomPacketTest() {
  byte i;
   unsigned long start, stop;

  start = millis();
  for (i=0;i<255;i++) {
    if (!randomPacketTest0()) 
      return false;
  }    
  stop = millis();

  throughput = (256.0 * 16.0 * 8.0) / ((stop - start) / 1000.0);
  return true;
}

void runHammingSerialTests() {
  Serial1.begin(115200,SERIAL_7N1);
  Serial2.begin(115200,SERIAL_7N1);
  printTestTitle("Hamming Serial Adapter");
  printTestResult("Send Byte Test",sendByteTest());
  printTestResult("Send 16 Byte Test",send16ByteTest());
  printTestResult("Random Packet Test",randomPacketTest());
  Serial.print("Throughput="); Serial.print(throughput); Serial.println(" bps");
  Serial.print("Errors Corrected="); Serial.print(slave.errorsCorrected); 
}


