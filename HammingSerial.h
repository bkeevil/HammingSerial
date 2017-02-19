#ifndef HammingSerial_h
#define HammingSerial_h

#include "Stream.h"
#include "HardwareSerial.h"
#include "Hamming.h"

#define MAX_PACKET_SIZE           16
#define PACKET_START_SYMBOL       127
#define PACKET_END_SYMBOL         0

#define PACKET_READ_STATE_UNKNOWN 0
#define PACKET_READ_STATE_START   1
#define PACKET_READ_STATE_DATA    2
#define PACKET_READ_STATE_END     3

class HammingSerial {
  private:
    HardwareSerial *_serial;
    bool readToBuffer();
    bool writeToBuffer();
    bool calcCRC();
    bool checkCRC();
    bool encodeBuffer();
    bool decodeBuffer();
    HammingCode hamming;
  public:
    HammingSerial(HardwareSerial *serial) : _serial(serial) { hamming.parityBit = false; hamming.serialInversion = true;}
    FifoBuffer rx_buffer;
    FifoBuffer tx_buffer;

    byte readState = PACKET_READ_STATE_UNKNOWN;
    uint16_t errorCount = 0;
    uint16_t errorsCorrected = 0;
 
    void event() { if (tx_buffer.count > 0) writeToBuffer(); }
    bool receive();
    bool send();    
};

// Reads a packet from the serial stream and returns true if the buffer contains a complete packet.
bool HammingSerial::readToBuffer() {
  byte b;
  byte in[2];

  if ((readState == PACKET_READ_STATE_END) && _serial->available()) {
    b = _serial->read();
    if (b == PACKET_START_SYMBOL) {
      readState = PACKET_READ_STATE_START;
    } else {
      readState = PACKET_READ_STATE_UNKNOWN;
      ++errorCount;
      rx_buffer.clear();
    }
  }
  
  while ((readState == PACKET_READ_STATE_UNKNOWN) && _serial->available()) {
    b = _serial->read();
    if (b == PACKET_START_SYMBOL) {
      readState = PACKET_READ_STATE_START;
    } 
  }
  
  if (readState == PACKET_READ_STATE_START) {
    rx_buffer.clear();
    readState = PACKET_READ_STATE_DATA;
  }

  if (readState == PACKET_READ_STATE_DATA) {
    while (_serial->available() && (readState == PACKET_READ_STATE_DATA)) {
      b = _serial->read();
      if (b == PACKET_END_SYMBOL) {
        if (_serial->available() && (_serial->peek() != PACKET_START_SYMBOL)) {
          rx_buffer.push(b);
        } else {
          if ((rx_buffer.count % 2) == 1) {
            readState = PACKET_READ_STATE_UNKNOWN;
            errorCount++;
            rx_buffer.clear();
          } else {
            readState = PACKET_READ_STATE_END;
          }  
        }
      } else {
        if (rx_buffer.isFull()) {
          errorCount++;
          rx_buffer.clear();
          readState = PACKET_READ_STATE_UNKNOWN;
        } else {
          rx_buffer.push(b);
        }   
      }  
    }
  }

  return (readState == PACKET_READ_STATE_END);
  
}

bool HammingSerial::writeToBuffer() {
  byte b;
  while ((tx_buffer.count > 0) && (_serial->availableForWrite() < SERIAL_TX_BUFFER_SIZE)) {
    tx_buffer.pop(&b);
    _serial->write(b);    
  }
  return (tx_buffer.count == 0);
}

// Returns false if the packet size > MAX_PACKET_SIZE, otherwise calculates the CRC on the transmit buffer content and appends it to the buffer.
bool HammingSerial::calcCRC() {
  byte crc = 0;
  byte b,i,cnt;

  cnt = tx_buffer.count;
  if (cnt <= MAX_PACKET_SIZE) {
    for (i=0;i<cnt;i++) {
      tx_buffer.pop(&b);
      crc ^= b;    
      tx_buffer.push(b);
    }
    tx_buffer.push(crc);
    return true;
  } else {
    return false;   
  }
}

// Checks the CRC on the recieve buffer content. Removes the crc value from the buffer.  Returns true if the crc was valid.
bool HammingSerial::checkCRC() {
  byte crc = 0;
  byte b,i,cnt;

  cnt = rx_buffer.count;
  for (i=0;i<cnt-1;i++) {
    rx_buffer.pop(&b);
    crc ^= b;    
    rx_buffer.push(b);
  }
  rx_buffer.pop(&b);
  if (b == crc) {
    return true;
  } else {
    rx_buffer.clear();
    return false;
  }    
}

// Hamming encodes the contents of the buffer and stores it in the buffer.
bool HammingSerial::encodeBuffer() {
  byte enc[2];
  byte dec; 
  byte i,cnt; 

  cnt = tx_buffer.count;
  if (cnt <= MAX_PACKET_SIZE + 1) {  // 16 bytes plus crc
    tx_buffer.push(PACKET_START_SYMBOL);
    for (i=0;i<cnt;i++) {
      tx_buffer.pop(&dec);
      hamming.fastEncode(dec,enc);
      tx_buffer.push(enc[0]);
      tx_buffer.push(enc[1]);
    }
    tx_buffer.push(PACKET_END_SYMBOL);
    return true;
  } else {
    tx_buffer.clear();
    return false;
  }
}

// Hamming decodes the contents of the buffer and stores it in buffer.  
bool HammingSerial::decodeBuffer() {
  byte enc[2];
  byte dec; 
  byte i,cnt;

  cnt = rx_buffer.count;
  if ((cnt % 2) == 1) {
    rx_buffer.clear();
    ++errorCount;
    return false;
  } else {
    for (i=0;i<(cnt / 2);i++) {
      rx_buffer.pop(&enc[0]);
      rx_buffer.pop(&enc[1]);
      if (hamming.decode(enc,&dec)) {
        if (hamming.errorCorrected) 
          ++errorsCorrected;
        rx_buffer.push(dec);
      } else {
        ++errorCount;
        rx_buffer.clear();
        return false;        
      }
    }  
  }
  return true;
}

bool HammingSerial::send() {
  if (calcCRC()) {
    //Serial.println("CalcCRC");
    //tx_buffer.dump();
    if (encodeBuffer()) {
      //Serial.println("EncodeBuffer");
      //tx_buffer.dump();
      if (writeToBuffer()) {
        //Serial.println("WriteToBuffer");
        return true;
      }
    }
  }
  return false;
}  

bool HammingSerial::receive() {
  if (readToBuffer()) {
    //Serial.println("ReadToBuffer");
    //rx_buffer.dump();
    if (decodeBuffer()) {
      //Serial.println("decodeBuffer");
      //rx_buffer.dump();
      if (checkCRC()) {
        //Serial.println("checkCRC");
        //rx_buffer.dump();
        return true;
      }
    }
  }  
  return false;
}

#endif

