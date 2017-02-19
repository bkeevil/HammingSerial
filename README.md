# HammingSerial
A Hamming(7,4) encoded serial port with a 16 byte packet size.  Includes a Hamming Encoder/Decoder and Fifo Buffer.

HammingCode class: (Implements Hamming(7,4) error correction protocol.  Look it up in Wikipedia.)

  serialInversion - Set to true to invert the 3rd and 7th bit.  This makes the bit stream suitable for devices that have trouble with more than 4 consecutive bits.  Both sender and receiver must have the same setting.

  parityBit - When parity bit is true, the encoder can detect a 2 bit error and correct a one bit error (at the cost of an extra bit).  When false, it can detect and correct a one bit error only.
  
  errorCorrected - decode() sets this to true if it corrects an error, false otherwise.  Used to see if errors are being corrected.
  
  enocde() - encodes in into out.  If parity is false then out[] contains 7 bit data (the 8th bit is a parity bit)
  
  decode() - decodes in into out.  Returns true if the conversion was a success, false otherwise.  
  
  fastEncode() - same as encode() but uses a lookup table, which is significantly faster.  Encode() is retained for interest sake.
  
FifoBuffer class: (First In, First Out Buffer)

  count - The number of bytes in the buffer.  Max size is FIFO_BUFFER_SIZE.
   
  isFull() - Returns true if the buffer is full
  
  push() - adds a byte to the end of the buffer and returns true if successful.
  
  peek() - Takes the first byte in the buffer and stores it in value but does not remove it from the buffer.  Returns true if successful.
  
  pop() - Takes the first byte in the buffer and stores it in value, removing it from the buffer.  Returns true if successful.
  
  clear() - Clears the buffer.
  
HammingSerial class: (Implements an Hamming Encode Serial Port with 8 bit CRC error detection and packetization)

To send a packet, push() the data onto the tx_buffer and then call send()
To receive packets, call receive().  If the result is true then rx_buffer contains a complete packet.
Somewhere in the main program loop, you need to call event(), which flushes the tx_buffer.

  HammingSerial(serial) - Pass a HardwareSerial object to the constructor to tell it which Arduino serial port to use.
  
  rx_buffer - A FifoBuffer which recieves a decoded packet
  
  tx_buffer - A FifoBuffer to assemble a packet for encoding and sending
  
  receive() - reads data from the serial port, decodes it, and places the data in rx_buffer.  Returns true when a complete packet exists in the buffer.
  
  send() - Encodes the data in tx_buffer and sends it out on the serial port.  
  
  event() - Must be called in program loop to ensure data is transmitted on the serial port.  
  
