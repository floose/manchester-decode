#include <stdio.h>
#include <stdint.h>

//
//Defines
//
#define BUFFER_SIZE 48
#define SYMBOL_SIZE 6
#define THRESHOLD 2049

//each manchester symbols has 6 adc samples
//the entire buffer holds 48 symbols, thus 8 bits
//hence, for now each interaction must decode 1 byte

//
//Globals
//
volatile uint16_t buffer[BUFFER_SIZE];
volatile uint8_t buffer_index = 0;

//
//Prototypes of functions
//
uint8_t process_sample(uint16_t sample);
uint8_t process_bit(uint8_t bit_value);

//
//main function
//
int main()
{

  // Simulates some samples for testing purposes
  // 10 bits of data in the for of
  // 01 00 00 11 = C char
  // + 2 dummy bits that are perfect 0s
  // Thus the entire sequence is:
  // 01 00 00 11 to be detected
  uint16_t samples[] = {
      //199,199,199,4000,4000,4000,
      199,199,199,4000,4000,4000,
      4000,4000,4000,199,199,199,
      199,199,199,4000,4000,4000,
      199,199,199,4000,4000,4000,
      199,199,199,4000,4000,4000,
      199,199,199,4000,4000,4000,
      4000,4000,4000,199,199,199,
      4000,4000,4000,199,199,199,
      //199,199,199,4000,4000,4000,
  };

  int x = sizeof(samples) / sizeof(samples[0]);
  printf("number of samples of buffer: %d\n",x);

  // fill in the samples
  for (int i = 0; i < sizeof(samples) / sizeof(samples[0]); i++)
  {
    buffer[buffer_index] = samples[i];
    buffer_index = (buffer_index + 1) % BUFFER_SIZE; //circular buffer wraper


    // processes each sample during each adc conversion
    printf("Processing sample number %d = %d\n",i,samples[i]);
    process_sample(samples[i]);
  }

  return 0;
}

uint8_t process_sample(uint16_t sample)
{
  static uint16_t last_sample = 0;
  static uint8_t loopcount = 0; //control variable, erase it afterwards
  static uint8_t bit_value = 0;
  loopcount++; //loop counter increment

  // Check for a transition from high to low or low to high
  if ((sample > THRESHOLD && last_sample < THRESHOLD))
  {
    printf("Here I found one transition from low to high.\n");
    bit_value = 0; 
  }
  else if((sample < THRESHOLD && last_sample > THRESHOLD))
  {
    printf("Here I found one transition from high to low.\n");
    bit_value = 1;
  }  

  if(loopcount % SYMBOL_SIZE == 0)//checks if reached end of symbol
    {
      printf(".............................Symbol ended. Processing bit is %d\n", bit_value);
      process_bit(bit_value); 
    }
  last_sample = sample; //holds past samples
}

uint8_t process_bit(uint8_t bit_value)
{
  static uint8_t bits_received = 0;
  static uint8_t decoded_byte = 0;
  // checks if a full byte is received, then prints it on the console
  if (bit_value == 1)
  {
    decoded_byte = decoded_byte << 1; // left shift
    decoded_byte = decoded_byte | 0x01; 
  }
  else if(bit_value == 0)
  {
    decoded_byte = (decoded_byte << 1); // Append a 0 to the byte
  }

  bits_received++;

  if (bits_received == 8)
  {
    printf("Received byte: %c\n", decoded_byte);
    bits_received = 0;
    decoded_byte = 0;
  }

}