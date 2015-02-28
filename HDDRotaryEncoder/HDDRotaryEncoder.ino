/*
 @see http://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino
 Rotary encoder read example 
*/
#define ENC_A 14
#define ENC_B 15
#define ENC_PORT PINC
#define PROCESSING 1

volatile int counter = 0; // changed by encoder input
volatile byte ab = 0; // The previous & current reading
 
void setup()
{
  /* Setup encoder pins as inputs */
  pinMode(ENC_A, INPUT);
  digitalWrite(ENC_A, HIGH);
  pinMode(ENC_B, INPUT);
  digitalWrite(ENC_B, HIGH);
  Serial.begin (57600);
  Serial.println("Start");
  
  setupPinInterrupt();
}
 
void loop()
{

  static int last_count = 0;
  static byte last_ab = 0;
  
  if (last_ab != ab) {
    last_ab = ab;
    if (!PROCESSING) {
      Serial.print("Counter: ");
      Serial.print(( ab & 0x0f ), DEC);
      Serial.print(" : ");
      Serial.print(( ab & 0x0f ), BIN);
      Serial.print(" : ");
      Serial.println(counter, DEC);
    }
  }
  
  if (last_count != counter) {
    last_count = counter;
    if (PROCESSING) {
      Serial.println(counter, DEC);
    }
  }
  
  
}
 
// Pin interrupt on port C == A0 -> A5 
// Any change on any enabled PCINT[14:8] pin will cause an interrupt.
ISR(PCINT1_vect)
{
  char tmpdata;
  tmpdata = read_encoder();
  if (tmpdata) {
    counter += tmpdata;
  }
}

void setupPinInterrupt()
{
  // 13.2.4 PCICR – Pin Change Interrupt Control Register
  // Bit 1 – PCIE1: Pin Change Interrupt Enable 1
  PCICR = (1 << PCIE1); // 0x02  00000010
  
  // 13.2.7 PCMSK1 – Pin Change Mask Register 1
  PCMSK1 =  ((1 << PCINT9) | (1 << PCINT8)); // listen for interrupts on A1 and A0 
}

/**
 * returns change in encoder state (-1,0,1) 
 */
int8_t read_encoder()
{
  // enc_states[] array is a look-up table; 
  // it is pre-filled with encoder states, 
  // with “-1″ or “1″ being valid states and “0″ being invalid. 
  // We know that there can be only two valid combination of previous and current readings of the encoder 
  // – one for the step in a clockwise direction, 
  // another one for counterclockwise. 
  // Anything else, whether it's encoder that didn't move between reads 
  // or an incorrect combination due to bouncing, is reported as zero.
  static int8_t enc_states[] = {
    0,-1,1,0, 1,0,0,-1, -1,0,0,1, 0,1,-1,0
  };
  
  /*
   The lookup table of the binary values represented by enc_states 
     ___     ___     __
   A    |   |   |   |
        |___|   |___|
      1 0 0 1 1 0 0 1 1
      1 1 0 0 1 1 0 0 1
     _____     ___     __
   B      |   |   |   |
          |___|   |___|
   
   A is represented by bit 0 and bit 2
   B is represented by bit 1 and bit 3
   With previous and current values stored in 4 bit data there can be
   16 possible combinations.
   The enc_states lookup table represents each one and what it means:
   
   [0] = 0000; A & B both low as before: no change : 0
   [1] = 0001; A just became high while B is low: reverse : -1
   [2] = 0010; B just became high while A is low: forward : +1
   [3] = 0011; B & A are both high after both low: invalid : 0
   [4] = 0100; A just became low while B is low: forward : +1
   [5] = 0101; A just became high after already being high: invalid : 0
   [6] = 0110; B just became high while A became low: invalid : 0
   [7] = 0111; A just became high while B was already high: reverse : -1
   [8] = 1000; B just became low while A was already low: reverse : -1
   etc...
   
   Forward: 1101 (13) - 0100 (4) - 0010 (2) - 1011 (11)
   Reverse: 1110 (14) - 1000 (8) - 0001 (1) - 0111 (7)
   
  */

  // ab gets shifted left two times 
  // saving previous reading and setting two lower bits to “0″ 
  // so the current reading can be correctly ORed.
  ab <<= 2;
  
  // ENC_PORT & 0×03 reads the port to which encoder is connected 
  // and sets all but two lower bits to zero 
  // so when you OR it with ab bits 2-7 would stay intact. 
  // Then it gets ORed with ab. 
  ab |= ( ENC_PORT & 0x03 );  //add current state
  // At this point, we have previous reading of encoder pins in bits 2,3 of ab, 
  // current readings in bits 0,1, and together they form index of (AKA pointer to) enc_states[]  
  // array element containing current state.
  // The index being the the lowest nibble of ab (ab & 0x0f)
  return ( enc_states[( ab & 0x0f )]);
}

 
