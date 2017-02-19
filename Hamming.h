#ifndef Hamming_h
#define Hamming_h

const byte generatorMatrix[7][4] = { 
  {1,1,0,1},
  {1,0,1,1},
  {1,0,0,0},
  {0,1,1,1},
  {0,1,0,0},
  {0,0,1,0},
  {0,0,0,1}
};

const byte parityCheckMatrix[3][7] = {
  {1,0,1,0,1,0,1},
  {0,1,1,0,0,1,1},
  {0,0,0,1,1,1,1}
};

const byte encodeTable[16] = {0,135,153,30,170,45,51,180,75,204,210,85,225,102,120,255};

class HammingCode {
  private:
    byte _dataVector[4];
    byte _transmitVector[8];
    byte _receiveVector[8];
    byte _syndromeVector[3];
    byte _decodeVector[4];
    void _encode();
    byte _calcSyndromeVector();
    bool _decode();
  public:
    bool serialInversion = false;
    bool parityBit = false;
    bool errorCorrected = false;
    void encode(byte in, byte out[2]);
    bool decode(byte in[2], byte *out);
    void fastEncode(byte in, byte out[2]);
};

void HammingCode::_encode() {
  byte p = 0;
  for (byte y=0;y<7;y++) {
    _transmitVector[y] = 0;
    for (byte x=0;x<4;x++) {
      _transmitVector[y] += generatorMatrix[y][x] * _dataVector[x];     
    }
    _transmitVector[y] = _transmitVector[y] % 2;
    if (_transmitVector[y] > 0) p++;
  }
  
  // add parity bit 
  if (parityBit) {
    _transmitVector[7] = p % 2;
  } else {
    _transmitVector[7] = 0;
  }
    
  // invert bits for serial transfer
  if (serialInversion) {
    _transmitVector[3] = (_transmitVector[3] + 1) % 2;
    if (parityBit) {
      _transmitVector[7] = (_transmitVector[7] + 1) % 2;
    }
  }    
}

void HammingCode::encode(byte in, byte out[2]) {
  _dataVector[0] = (in & 1);
  _dataVector[1] = (in & 2) >> 1;
  _dataVector[2] = (in & 4) >> 2;
  _dataVector[3] = (in & 8) >> 3;
  _encode();
  out[0] = (_transmitVector[0] + (_transmitVector[1] * 2) + (_transmitVector[2] * 4) + (_transmitVector[3] * 8) + (_transmitVector[4] * 16) + (_transmitVector[5] * 32) + (_transmitVector[6] * 64) + (_transmitVector[7] * 128));

  _dataVector[0] = (in & 16) >> 4;
  _dataVector[1] = (in & 32) >> 5;
  _dataVector[2] = (in & 64) >> 6;
  _dataVector[3] = (in & 128) >> 7;
  _encode();
  out[1] = (_transmitVector[0] + (_transmitVector[1] * 2) + (_transmitVector[2] * 4) + (_transmitVector[3] * 8) + (_transmitVector[4] * 16) + (_transmitVector[5] * 32) + (_transmitVector[6] * 64) + (_transmitVector[7] * 128));
}

byte invertBit(byte val, byte bitnum) {
  if (bitRead(val,bitnum))
    return bitClear(val,bitnum);
  else
    return bitSet(val,bitnum); 
}

void HammingCode::fastEncode(byte in, byte out[2]) {
  // Look up the corresponding code for high and low nibbles from table
  out[0] = encodeTable[in & 15];
  out[1] = encodeTable[in >> 4];

  // Reset parity bit to zero if not required.  Encoding table contains parity bits.
  if (!parityBit) {
    out[0] = out[0] & 127;
    out[1] = out[1] & 127;
  }
  
  // invert bits for serial transfer
  if (serialInversion) {
    out[0] = invertBit(out[0],3);
    out[1] = invertBit(out[1],3);
    if (parityBit) {
      out[0] = invertBit(out[0],7);
      out[1] = invertBit(out[1],7);
    }
  }      
}

byte HammingCode::_calcSyndromeVector() {
  for (byte y=0;y<3;y++) {
    _syndromeVector[y] = 0;
    for (byte x=0;x<7;x++) {
      _syndromeVector[y] += parityCheckMatrix[y][x] * _receiveVector[x];     
    }
    _syndromeVector[y] = _syndromeVector[y] % 2;
  }
  byte e = (_syndromeVector[0] * 1) + (_syndromeVector[1] * 2) + (_syndromeVector[2] * 4);  
  return e;
}

bool HammingCode::_decode() {
  // Invert Bits for Serial Transfer
  if (serialInversion) {
    _receiveVector[3] = (_receiveVector[3] + 1) % 2;
    if (parityBit) {
      _receiveVector[7] = (_receiveVector[7] + 1) % 2; 
    }
  }  
  byte e=_calcSyndromeVector();
  if (e > 0) {
    _receiveVector[e-1] = (_receiveVector[e-1] + 1) % 2;
    e = _calcSyndromeVector();  
    if (e==0) {
      errorCorrected = true;
    } else {
      errorCorrected = false;
      return false;
    }
  }

  // Check Parity Bit
  if (parityBit) {
    byte p=0;
    for (byte b=0;b<8;b++) {
      p += _receiveVector[b];
    }
    if (p % 2 == 1) {
      errorCorrected = false;
      return false;
    }
  }
   
  _decodeVector[0] = _receiveVector[2];
  _decodeVector[1] = _receiveVector[4];
  _decodeVector[2] = _receiveVector[5];
  _decodeVector[3] = _receiveVector[6];  
  return true;  
}

bool HammingCode::decode(byte in[2], byte *out) {
  errorCorrected = false;
  _receiveVector[0] = in[0] & 1;
  _receiveVector[1] = (in[0] & 2) >> 1;
  _receiveVector[2] = (in[0] & 4) >> 2;
  _receiveVector[3] = (in[0] & 8) >> 3;
  _receiveVector[4] = (in[0] & 16) >> 4;
  _receiveVector[5] = (in[0] & 32) >> 5;
  _receiveVector[6] = (in[0] & 64) >> 6;
  _receiveVector[7] = (in[0] & 128) >> 7;
  if (_decode()) {
    *out = (_decodeVector[0] + (_decodeVector[1] * 2) + (_decodeVector[2] * 4) + (_decodeVector[3] * 8));
    _receiveVector[0] = in[1] & 1;
    _receiveVector[1] = (in[1] & 2) >> 1;
    _receiveVector[2] = (in[1] & 4) >> 2;
    _receiveVector[3] = (in[1] & 8) >> 3;
    _receiveVector[4] = (in[1] & 16) >> 4;
    _receiveVector[5] = (in[1] & 32) >> 5;
    _receiveVector[6] = (in[1] & 64) >> 6;
    _receiveVector[7] = (in[1] & 128) >> 7;
    if (_decode()) {
      *out += ((_decodeVector[0] * 16) + (_decodeVector[1] * 32) + (_decodeVector[2] * 64) + (_decodeVector[3] * 128));   
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

/*
void printDataVector() {
  Serial.print("{");
  Serial.print(dataVector[0]); Serial.print(",");
  Serial.print(dataVector[1]); Serial.print(",");
  Serial.print(dataVector[2]); Serial.print(",");
  Serial.print(dataVector[3]); Serial.print("}");
}

void printDecodeVector() {
  Serial.print("{");
  Serial.print(dataVector[0]); Serial.print(",");
  Serial.print(dataVector[1]); Serial.print(",");
  Serial.print(dataVector[2]); Serial.print(",");
  Serial.print(dataVector[3]); Serial.print("}");
}

void printTransmitVector() {
  Serial.print("{");
  Serial.print(transmitVector[0]); Serial.print(",");
  Serial.print(transmitVector[1]); Serial.print(",");
  Serial.print(transmitVector[2]); Serial.print(",");
  Serial.print(transmitVector[3]); Serial.print(",");
  Serial.print(transmitVector[4]); Serial.print(",");
  Serial.print(transmitVector[5]); Serial.print(",");
  Serial.print(transmitVector[6]); Serial.print(",");  
  Serial.print(transmitVector[7]); Serial.print("}");
}

void printReceiveVector() {
  Serial.print("{");
  Serial.print(receiveVector[0]); Serial.print(",");
  Serial.print(receiveVector[1]); Serial.print(",");
  Serial.print(receiveVector[2]); Serial.print(",");
  Serial.print(receiveVector[3]); Serial.print(",");
  Serial.print(receiveVector[4]); Serial.print(",");
  Serial.print(receiveVector[5]); Serial.print(",");
  Serial.print(receiveVector[6]); Serial.print(",");  
  Serial.print(receiveVector[7]); Serial.print("}");
}
*/

#endif
