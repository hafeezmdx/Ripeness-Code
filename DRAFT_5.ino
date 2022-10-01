#include <Stepper.h>

#define BUTTON_PIN 12     // the button and 10K pulldown are connected to 12
int SENSOR_fsr = A0;     // the FSR and 10K pulldown are connected to a0
int sensors;     // the analog reading from the FSR resistor divider
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

// for your motor
const int stepPin = 3; //Y.STEP
const int dirPin = 6; // Y.DIR

byte lastButtonState = LOW;
byte stepperState = LOW;

unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;

int stepCount = 0;  // number of steps the motor has taken

enum State_enum {STOP, TOUCH_DETECTED, PROBE_FORWARD, PROBE_BACK};

uint8_t state = STOP;

void state_machine_run();
void motors_stop();
void motors_forward();
void motors_probe();
void motors_back();

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(SENSOR_fsr, INPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void state_machine_run() {

  sensors = analogRead(SENSOR_fsr);
  Serial.print("Analog reading = ");
  Serial.print(sensors);     // the raw analog reading
  delay(1000);
  switch (state)
  {
    case STOP:
      if (millis() - lastTimeButtonStateChanged > debounceDuration) {
        byte buttonState = digitalRead(BUTTON_PIN);
        if (buttonState != lastButtonState) {
          lastTimeButtonStateChanged = millis();
          lastButtonState = buttonState;
          if (buttonState == LOW) {
            motors_forward();
            state = TOUCH_DETECTED;
          }
        }
      }
      break;

    case TOUCH_DETECTED:
      if (sensors >= 0) {
        motors_stop();
        state = PROBE_FORWARD;
      }
      break;

    case PROBE_FORWARD:

      digitalWrite(dirPin, LOW);
      for (int x = 0; x < 100; x++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
        motors_back();
        state = PROBE_BACK;
      }
      break;

    case PROBE_BACK:
       digitalWrite(dirPin, HIGH);
      for (int x = 0; x < 200; x++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
        motors_back();
        state = STOP;
      }
      break;
  }
}
void loop()
{
  state_machine_run();
}

void motors_forward()
{
  //code for driving forward
  stepperState = (stepperState == HIGH) ? LOW : HIGH;
  digitalWrite(dirPin, LOW); // Enables the motor to move in a particular direction
  for (int x = 0; x < 200; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
}
void motors_stop()
{
  //code for stopping motor
  digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction
  for (int x = 0; x < 0; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
}
void motors_probe()
{
  if (sensors > 740) {
    Serial.println(" - ripped peach");
  } else if (sensors < 750) {
    Serial.println(" - unripped peach");
  } 
  delay(1000);


}
void motors_back()
{
  if (sensors >= 0) {
    Serial.println(" - Moving back");
  }
}
