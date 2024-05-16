#include <Arduino.h>
/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-rotary-encoder
 */

// #include <ezButton.h>  // the library to use for SW pin

#define CLK_PIN 2
#define DT_PIN 3

#define LED_PIN 4
#define NB_PULSE_BEFORE_LIGHT_ON 1500
#define LED_TIME_ON 50 // ms OFF pour l'essai

#define DIRECTION_CW 0  // clockwise direction
#define DIRECTION_CCW 1 // counter-clockwise direction

volatile int counter = 0;
volatile int direction = DIRECTION_CW;
volatile unsigned long last_time; // for debouncing
int prev_counter;

// ezButton button(SW_PIN);  // create ezButton object that attach to pin 4

void ISR_encoderChange()
{
  if ((micros() - last_time) < 50) // debounce time is 50ms
    return;

  if (digitalRead(DT_PIN) == HIGH)
  {
    // the encoder is rotating in counter-clockwise direction => decrease the counter
    counter--;
    direction = DIRECTION_CCW;
  }
  else
  {
    // the encoder is rotating in clockwise direction => increase the counter
    counter++;
    direction = DIRECTION_CW;
  }

  last_time = micros();
}

void setup()
{
  Serial.begin(9600);

  // configure encoder pins as inputs
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  // button.setDebounceTime(50);  // set debounce time to 50 milliseconds

  // use interrupt for CLK pin is enough
  // call ISR_encoderChange() when CLK pin changes from LOW to HIGH
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), ISR_encoderChange, RISING);

  digitalWrite(LED_PIN, HIGH);
}

void loop()
{
  // button.loop();  // MUST call the loop() function first

  while (true)
    if (prev_counter != counter)
    {
      Serial.print("DIRECTION: ");
      if (direction == DIRECTION_CW)
        Serial.print("horaire");
      else
        Serial.print("anti-horaire");

      Serial.print(" | Compteur: ");
      Serial.println(counter);

      prev_counter = counter;

      if (counter > NB_PULSE_BEFORE_LIGHT_ON)
      {
        Serial.println("LED OFF");
        digitalWrite(LED_PIN, LOW);
        delay(LED_TIME_ON);
        counter = 0;
        digitalWrite(LED_PIN, HIGH);
      }
    }
}
