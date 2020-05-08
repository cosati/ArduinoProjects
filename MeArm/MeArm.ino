#include <Servo.h>

Servo m_claw, m_base, m_right, m_left;

int x_key = A1;
int x_pos;
int y_key = A0;
int y_pos;
int pos_right = 120;
int pos_left = 130;
int base;
int claw;
int btn = 3;
int base_pin = 2;
int claw_pin = 3;
int steps = 5;

void setup() {
  Serial.begin(9600);
  m_claw.attach(7);
  m_base.attach(6);
  m_right.attach(5);
  m_right.write(pos_right);
  m_left.attach(4);
  m_left.write(pos_left);
  pinMode(btn, INPUT);
  pinMode(x_key, INPUT);  
  pinMode(y_key, INPUT);  
}

void loop() {

  Serial.print("MD: ");
  Serial.print(pos_right);
  Serial.print(" | ME: ");
  Serial.print(pos_left);
  Serial.print(" | Base: ");
  Serial.print(base);
  Serial.print(" | Garra: ");
  Serial.print(claw);
  Serial.print(" | JSX: ");
  Serial.print(analogRead(x_key));
  Serial.print(" | JSY: ");
  Serial.print(analogRead(y_key));
  Serial.println();

//  if (digitalRead(btn) == HIGH) {
//    pos_right = 120;
//    pos_left = 130;
//  }

  x_pos = analogRead(x_key);
  y_pos = analogRead(y_key);
  base = analogRead(base_pin);
  base = map(base, 0, 1023, 0, 180);
  claw = analogRead(claw_pin);
  claw = map(claw, 0, 1023, 0, 47);

  m_base.write(base);
  m_claw.write(claw);

  if (analogRead(x_key) < 490) {
    pos_right += steps;
    Serial.println(pos_right);
  }
  else if (analogRead(x_key) > 510 && pos_right >= 45) {
    pos_right -= steps;
    Serial.println(pos_right);
  }
  if (analogRead(y_key) < 480) {
    pos_left += steps;
    Serial.println(pos_left);
  }
  else if (analogRead(y_key) > 495 && pos_left >= 35) {
    pos_left -= steps;
    Serial.println(pos_left);
  }
  
  m_right.write(pos_right);
  m_left.write(pos_left);

  delay(15);
  
}
