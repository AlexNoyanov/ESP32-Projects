// __/\\\\\\\\\\\\\\\_____/\\\\\\\\\\\____/\\\\\\\\\\\\\_______/\\\\\\\\\\_____/\\\\\\\\\_____
// _\/\\\///////////____/\\\/////////\\\_\/\\\/////////\\\___/\\\///////\\\__/\\\///////\\\___
//   _\/\\\______________\//\\\______\///__\/\\\_______\/\\\__\///______/\\\__\///______\//\\\__
//   _\/\\\\\\\\\\\_______\////\\\_________\/\\\\\\\\\\\\\/__________/\\\//_____________/\\\/___
//     _\/\\\///////___________\////\\\______\/\\\/////////___________\////\\\_________/\\\//_____
//     _\/\\\_____________________\////\\\___\/\\\_______________________\//\\\_____/\\\//________
//       _\/\\\______________/\\\______\//\\\__\/\\\______________/\\\______/\\\____/\\\/___________
//       _\/\\\\\\\\\\\\\\\_\///\\\\\\\\\\\/___\/\\\_____________\///\\\\\\\\\/____/\\\\\\\\\\\\\\\_
//         _\///////////////____\///////////_____\///________________\/////////_____\///////////////__

// Ultra fast data transfer on ESP32 via Digital Pin
// By Alex Noyanov, Jan 7th 2025

// Logo: https://ascii.co.uk/text
// Text: Slant Relief

// ESP32 Connections:
// - GPIO23 -> Data output line
// - GPIO22 -> Trigger input (with internal pull-up)
// - GPIO2  -> Built-in LED (for visual feedback)
// - GND    -> Ground connection

// Ultra fast data transfer on ESP32 via Digital Pin
// For ESP32-WROOM-32 NodeMCU board

// Define the pins
#define DATA_PIN 23    // GPIO23 for data output
#define TRIGGER_PIN 22 // GPIO22 for trigger output
#define LED_PIN 2      // Built-in LED on GPIO2

#define LED_BLINK_DELAY 1000 // 0.05 seconds

#define TRIGGER_DELAY_START 1000 // Delay after starting transmission data
#define TRIGGER_DELAY_END 1000   // Delay after edning transmission

#define NEXT_TRANSMISSION_DELAY 2000 // Delay before next transition in LOOP

// Milliseconds (using delay())
// delay(1);     // 0.001 seconds
// delay(10);    // 0.01 seconds
// delay(100);   // 0.1 seconds
// delay(1000);  // 1 second

// Include required header for GPIO registers
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

// Define delay between bits (in microseconds)
#define DELAY_BETWEEN_BITS 1000 // Adjust this value as needed

void IRAM_ATTR sendByte(uint8_t byte)
{
  portDISABLE_INTERRUPTS();
  REG_WRITE(GPIO_OUT_W1TC_REG, (1 << TRIGGER_PIN));

  for (int i = 7; i >= 0; i--)
  {
    if (byte & (1 << i))
    {
      REG_WRITE(GPIO_OUT_W1TS_REG, (1 << DATA_PIN));
    }
    else
    {
      REG_WRITE(GPIO_OUT_W1TC_REG, (1 << DATA_PIN));
    }
    delay(1000);
    // delayMicroseconds(DELAY_BETWEEN_BITS); // Arduino standard function
  }

  REG_WRITE(GPIO_OUT_W1TC_REG, (1 << DATA_PIN));
  REG_WRITE(GPIO_OUT_W1TS_REG, (1 << TRIGGER_PIN));
  portENABLE_INTERRUPTS();
}

void setup()
{
  Serial.begin(115200);

  // Configure pins
  pinMode(DATA_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT); // Trigger is now an output
  pinMode(LED_PIN, OUTPUT);

  // Initial states
  digitalWrite(DATA_PIN, LOW);
  digitalWrite(TRIGGER_PIN, HIGH); // Start with trigger HIGH
  digitalWrite(LED_PIN, LOW);

  // Indicate ready with LED blink
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(LED_BLINK_DELAY);
    digitalWrite(LED_PIN, LOW);
    delay(LED_BLINK_DELAY);
  }

  Serial.println("\nESP32 Byte Sender Ready!");
  Serial.printf("DATA_PIN: GPIO%d, TRIGGER_PIN: GPIO%d\n", DATA_PIN, TRIGGER_PIN);
}

void loop()
{
  uint8_t byteToSend = 0xA5; // Example byte: 10100101

  // Visual indicator
  digitalWrite(LED_PIN, HIGH);

  // Set trigger LOW to indicate start of transmission
  digitalWrite(TRIGGER_PIN, LOW); // START transmission
  // Small delay to ensure trigger is detected
  delay(TRIGGER_DELAY_START);

  // Send the byte
  sendByte(byteToSend);

  // Set trigger HIGH to indicate end of transmission
  digitalWrite(TRIGGER_PIN, HIGH);

  // Small delay to ensure trigger is detected
  delay(TRIGGER_DELAY_END); // END transmission

  // Visual indicator off
  digitalWrite(LED_PIN, LOW);

  // Debug output
  Serial.print("Byte ");
  Serial.print(byteToSend);
  Serial.println("sent!");

  // Wait before next transmission
  delay(NEXT_TRANSMISSION_DELAY);
}
