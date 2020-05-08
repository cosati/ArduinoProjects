#include <Servo.h>

#define STEPS 5

Servo m_claw, m_base, m_right, m_left;

int y_key_pin = A0;
int pin_x_key = A1;
int pin_height = A2;
int pin_claw = A3;
int pos_z = 125;
int pos_x = 80;
int pos_y;
int prev;
int claw;
int btn = 2;

void setup() {
  Serial.begin(9600);
  m_claw.attach(7);
  m_base.attach(6);
  m_base.write(pos_x);
  m_right.attach(5);
  m_right.write(pos_z);
  m_left.attach(4);
  pinMode(btn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(btn), reset, FALLING);
  digitalWrite(btn, HIGH);
  pinMode(pin_x_key, INPUT);  
  pinMode(y_key_pin, INPUT);  
}

void loop() {

  Serial.print("ME (y): ");
  Serial.print(pos_y);
  Serial.print(" | MD (z): ");
  Serial.print(pos_z);
  Serial.print(" | Base (x): ");
  Serial.print(pos_x);
  Serial.print(" | Garra: ");
  Serial.print(claw);
  Serial.print(" | JSX: ");
  Serial.print(analogRead(pin_x_key));
  Serial.print(" | JSY: ");
  Serial.print(analogRead(y_key_pin));
  Serial.print(" | BTN: ");
  Serial.print(digitalRead(btn));
  Serial.println();

  pos_y = analogRead(pin_height);
  pos_y = map(pos_y, 0, 1023, 42, 180);
  claw = analogRead(pin_claw);
  claw = map(claw, 0, 1023, 0, 47);

  m_left.write(pos_y);
  m_claw.write(claw);

  if (analogRead(pin_x_key) < 490 && (pos_z + STEPS) <= 180 && (pos_z + STEPS) <= getMax(pos_y)) {
    pos_z += STEPS;
    Serial.println(pos_z);
  }
  else if (analogRead(pin_x_key) > 510 && (pos_z - STEPS) >= getMin(pos_y)) {
    pos_z -= STEPS;
    Serial.println(pos_z);
  }
  if (analogRead(y_key_pin) < 470) {
    pos_x -= STEPS;
    Serial.println(pos_x);
  }
  else if (analogRead(y_key_pin) > 505) {
    pos_x += STEPS;
    Serial.println(pos_x);
  }

  if (pos_y < 105) {
    pos_z = (pos_z < minDir(pos_y) ? minDir(pos_y) : pos_z);
  }

  if (pos_y > 135) {
    pos_z = (pos_z > maxDir(pos_y) ? maxDir(pos_y) : pos_z);
  }
  
  m_right.write(pos_z);
  m_base.write(pos_x);

  delay(15);
  
}

void reset() {
  Serial.print("Reset! ----------------------------");
  pos_z = 120;
  pos_x = 80;
}

int getMax(int p_esq) {
  if (p_esq <= 135) {
    return 180;
  }
  else return (315 - p_esq);
}

int getMin(int p_esq) {
  if (p_esq >= 105) {
    return 60;
  }
  else return (165 - p_esq);
}

int minDir(int t) {
  return (170 - t);
}

int maxDir(int t) {
  return (280 - t);
}
