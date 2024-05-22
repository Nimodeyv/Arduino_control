#include <Arduino.h>
/*
 * Created by Nicolas Morand
 *
 * For SIMONIN SAS
 *
 */

// ENCODER
#define CLK_PIN 2
#define DT_PIN 3

// LAMPES LEDS
#define LED_PIN_HAUT 4
#define LED_PIN_BAS 10
#define LED_TIME_ON 50 // en ms OFF pour l'essai

// CAMERA
#define CAMERA_PIN_HAUT 8
#define CAMERA_PIN_BAS 12

// TIMING
long NEXT_TOP_HAUT = 3000;                                       // en encodeur position
const long NB_PULSE_ENTRE_2_PHOTOS = 5443;                       // en encodeur position
long NEXT_TOP_BAS = NEXT_TOP_HAUT + NB_PULSE_ENTRE_2_PHOTOS / 2; // en encodeur position
bool function_LED_PHOTO_Active = false;
const long DUREE_LED = 5000; // en microsecondes

volatile int counter = 0;
volatile unsigned long start_time; // pour donner le top LED et photo
volatile unsigned long last_time;  // pour le debouncing
int prev_counter;

void ISR_encoderChange()
{
  if ((micros() - last_time) < 50) // debounce time is 50ms
    return;

  if (digitalRead(DT_PIN) == HIGH)
  {
    // the encoder is rotating in counter-clockwise direction => decrease the counter
    counter--;
  }
  else
  {
    // the encoder is rotating in clockwise direction => increase the counter
    counter++;
  }
  last_time = micros();
}

void setup()
{
  Serial.begin(9600);

  // configure encoder pins as inputs
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(LED_PIN_HAUT, OUTPUT);
  pinMode(CAMERA_PIN_HAUT, OUTPUT);
  pinMode(LED_PIN_BAS, OUTPUT);
  pinMode(CAMERA_PIN_BAS, OUTPUT);

  // call ISR_encoderChange() when CLK pin changes from LOW to HIGH
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), ISR_encoderChange, RISING);

  digitalWrite(LED_PIN_HAUT, HIGH);
  digitalWrite(CAMERA_PIN_HAUT, LOW);
  digitalWrite(LED_PIN_BAS, HIGH);
  digitalWrite(CAMERA_PIN_BAS, LOW);
}

void start_LED_PHOTO(int LED, int CAM)
{
  function_LED_PHOTO_Active = true;
  start_time = micros();
  // Serial.println("Function started");
  Serial.println("LAMPES HAUT ALLUME (ROUGE ETEINT) - PHOTO CAMERA HAUT (VERT ALLUME)");
  digitalWrite(LED, LOW);
  digitalWrite(CAM, HIGH);
}

void stop_LED_PHOTO(int LED, int CAM)
{
  function_LED_PHOTO_Active = false;
  // Serial.println("Function stopped");
  Serial.println("LAMPES HAUT ETEINT (ROUGE ALLUME) - PHOTO CAMERA HAUT (VERT ETEINT)");
  digitalWrite(LED, HIGH);
  digitalWrite(CAM, LOW);
}

void loop()
{
  if (counter >= NEXT_TOP_HAUT && !function_LED_PHOTO_Active)
  {
    start_LED_PHOTO(LED_PIN_HAUT, CAMERA_PIN_HAUT);
    NEXT_TOP_HAUT += NB_PULSE_ENTRE_2_PHOTOS;
  }

  if (function_LED_PHOTO_Active && (micros() - start_time) >= DUREE_LED)
  {
    stop_LED_PHOTO(LED_PIN_HAUT, CAMERA_PIN_HAUT);
    stop_LED_PHOTO(LED_PIN_BAS, CAMERA_PIN_BAS);
  }

  if (counter >= NEXT_TOP_BAS && !function_LED_PHOTO_Active)
  {
    start_LED_PHOTO(LED_PIN_BAS, CAMERA_PIN_BAS);
    NEXT_TOP_BAS += NB_PULSE_ENTRE_2_PHOTOS;
  }

  Serial.println(counter);
  prev_counter = counter;
}
