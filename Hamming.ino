#include "Hamming.h"
#include "Hamming_Tests.h"
#include "FifoBuffer.h"
#include "FifoBuffer_Tests.h"
#include "HammingSerial.h"
#include "HammingSerial_Tests.h"



void setup() {
  Serial.begin(57600);
  //runFifoTests();
  //runHammingTests();
  runHammingSerialTests();
}


void loop() {

}
