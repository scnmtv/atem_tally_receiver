/*
  Custom nrf24L01 atem tally receiver module with arduino-pro

   - pins 2,3,4 - address pins
   - pin 5 - LED output with driver
  
  29.3.2018 - Ferbi

*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins CE & CSN
RF24 radio(9, 10);

// Topology
const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };              // Radio pipe addresses for the 2 nodes to communicate.

// node ID
uint8_t nodeID = 0;

void setup() {

  Serial.begin(115200);
  printf_begin();
  Serial.println(F("\n\rradio details: RECEIVER\n\r"));

  // Setup and configure rf radio
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);          // MIN, LOW, HIGH, MAX
  radio.setAutoAck(0);                    // Ensure autoACK is enabled
  //  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(0, 15);                // Smallest time between retries, max no. of retries
  radio.setPayloadSize(1);                // Here we are sending 1-byte payloads to test the call-response speed
  //  radio.openWritingPipe(pipes[1]);        // Both radios listen on the same pipes by default, and switch when writing
  radio.openReadingPipe(1, pipes[0]);
  radio.startListening();                 // Start listening
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging

  // pin setup
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, OUTPUT);

  // get ID
  nodeID = getNodeID();     // gre od 0 - pri shiftu
  Serial.print("\n\rnodeID: ");
  Serial.println(nodeID);

  // starting...
  Serial.println("starting node...");
  delay(2000);
}

void loop(void) {

  //uint8_t tt = receiveData();
  //Serial.println(tt, BIN);
  //Serial.println(nodeID);

  if( ((receiveData() & ( 1 << nodeID )) >> nodeID) == true) 
    tally(true);
  else 
    tally(false);

  // can be lowered
  delay(10);
}

void tally(bool state){

  //Serial.println(state);
  if (state) digitalWrite(5, HIGH); 
  else digitalWrite(5, LOW);
}

uint8_t getNodeID() {

  uint8_t temp = 0;
  for (int i = 0; i < 3; i++) {
    temp = digitalRead(4 - i) << i;
    nodeID = nodeID | temp;
  }
  //Serial.println(( B00000111 ^ nodeID),BIN);
  return (B00000111 ^ nodeID);
}

uint8_t receiveData() {

  // receiveing data  -> no ACK - multireceiver network
  byte pipeNo;
  byte gotByte;                                       // Dump the payloads until we've gotten everything
  while ( radio.available(&pipeNo)) {
    radio.read( &gotByte, 1 );
    // radio.writeAckPayload(pipeNo, &gotByte, 1 );
    // Serial.println("ACK sent!");
    //Serial.print("DATA received: ");
    //Serial.println(gotByte);
  }
  return gotByte;
}

