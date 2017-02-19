#include "Test_Framework.h"

HammingCode h;

/* Hamming Table from Wikipedia (LSB First, Reverse Bit Order)
 *  0000 = 0000000
 *  0001 = 0000111
 *  0100 = 1001100
 *  1100 = 0111100
 *  0010 = 0101010
 *  1010 = 1011010
 *  0110 = 1100110
 *  1110 = 0010110
 *  0001 = 1101001
 *  1001 = 0011001
 *  0101 = 0100101
 *  1101 = 1010101
 *  0011 = 1000011
 *  1011 = 0110011
 *  0111 = 0001111
 *  1111 = 1111111
 */
 
float fastEncodeRate;
float encodeRate;
float decodeRate;

void fastEncodeSpeedTest() {
  byte o[2];
  unsigned long start, stop;
  start = micros();
  for (int x=0;x<1024;x++) {
    h.fastEncode(x % 256,o);  
  }
  stop = micros();
  fastEncodeRate = 1000000.0 / (stop - start);  
}

void encodeSpeedTest() {
  byte o[2];
  unsigned long start, stop;
  start = micros();
  for (int x=0;x<1024;x++) {
    h.encode(x % 256,o);  
  }
  stop = micros();
  encodeRate = 1000000.0 / (stop - start);
}

void decodeSpeedTest() {
  byte o[2]; byte r;
  unsigned long start, stop;
  h.encode(43,o);
  //o[0]++;
  start = millis();  
  for (int x=0;x<1024;x++) {
    h.decode(o,r);  
  }
  stop = millis();
  decodeRate = 1000.0 / (stop - start); 
}

bool encodeDecodeTest() {
  byte r;
  byte o[2];
  int e=0;

  for (int x=0;x<=255;x++) {
    h.encode(x,o);
    if (!h.decode(o,&r)) return false;
    if (x !=r) return false;
    if (h.errorCorrected) return false;
  }  
  return true;
}

bool fastEncodeDecodeTest() {
  byte r;
  byte o[2];
  int e=0;

  for (int x=0;x<=255;x++) {
    h.fastEncode(x,o);
    if (!h.decode(o,&r)) return false;
    if (x != r) return false;
    if (h.errorCorrected) return false;
  }  
  return true;
}

bool oneBitErrorTest() {
  byte i,j,r;
  byte o[2];
  int e=0;

  for (int x=0;x<=255;x++) {
    h.encode(x,o);
    i = (x >> 4) & 1;
    j = x & 7;
    if (j ==7) j=random(7);
    //Serial.print("i="); Serial.print(i); Serial.print("j="); Serial.println(j);
    bitWrite(o[i],j,(bitRead(o[i],j) + 1) % 2);
    if (!h.decode(o,&r)) return false;
    if (!h.errorCorrected) return false;
  }  
  return true;
}

bool twoBitErrorTest() {
  byte i,j,r,a,b;
  byte o[2];
  int e=0;
  for (int x=0;x<=255;x++) {
    h.encode(x,o);
    i = (x >> 4) & 1;
    j = x & 7;
    if (j ==7) j=random(7);
    bitWrite(o[i],j,(bitRead(o[i],j) + 1) % 2);
    a=i;
    do
      b=random(7);
    while (b == j);
    bitWrite(o[a],b,(bitRead(o[a],b) + 1) % 2);    
    
    if (h.decode(o,&r)) return false;
    if (h.errorCorrected) return false;
  }  
  return true;
}

void printTable() {
  byte r;
  byte o[2];
  int e=0;

  for (int x=0;x<16;x++) {
    h.encode(x,o);
    Serial.print(x); Serial.print("="); Serial.println(o[0],BIN);
  }  
  return true;
}

void runHammingTests0() {
  printTestResult("Encode/Decode Test",encodeDecodeTest());
  printTestResult("Fast Encode/Decode Test",fastEncodeDecodeTest());
  printTestResult("1 Bit Error Test",oneBitErrorTest());
  if (h.parityBit) {
    printTestResult("2 Bit Error Test",twoBitErrorTest());
  }
  Serial.println();
  fastEncodeSpeedTest();
  encodeSpeedTest();
  decodeSpeedTest();
  Serial.print("Rates: Encoding= "); Serial.print(encodeRate); Serial.print(" KiB/s Fast Encoding="); Serial.print(fastEncodeRate); Serial.print(" KiB/s Decoding="); Serial.print(decodeRate); Serial.println(" KiB/s");
  Serial.print("Rates: Encoding= "); Serial.print(encodeRate*8); Serial.print(" Kbaud Fast Encoding="); Serial.print(fastEncodeRate*8); Serial.print(" Kbaud Decoding="); Serial.print(decodeRate*8); Serial.println(" Kbaud");
  Serial.println();
}

void runHammingTests() {
  printTestTitle("Hamming.h");
  printTestSubtitle("SET PARITY=OFF, SERIALINVERSION=OFF");
  h.parityBit = false;
  h.serialInversion = false;
  runHammingTests0();
  printTestSubtitle("SET PARITY=OFF, SERIALINVERSION=ON");
  h.parityBit = false;
  h.serialInversion = true;
  runHammingTests0();
  printTestSubtitle("SET PARITY=ON, SERIALINVERSION=OFF");
  h.parityBit = true;
  h.serialInversion = false;
  runHammingTests0();
  printTestSubtitle("SET PARITY=ON, SERIALINVERSION=ON");
  h.parityBit = true;
  h.serialInversion = true;
  runHammingTests0();

  h.parityBit = false;
  h.serialInversion = true;
  printTestSubtitle("Bit patterns for Serial Transmission");
  printTable();
}  
