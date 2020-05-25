#include <Servo.h>

#define STEPS 3

Servo m_claw, m_base, m_right, m_left;

unsigned long lastSwitchDetectedMillis;

int debounceInterval = 150;
int y_key_pin = A0;
int pin_x_key = A1;
int pin_height = A2;
int pin_claw = A3;
int pos_z = 125;
int pos_x = 80;
int pos_y;
int prev;
int claw;
int joystick = 2;
int btn = 3;
int time_delay = 15;
boolean manual = true;
boolean pause = false;

struct position {
  int servo_right;
  int servo_left;
  int servo_claw;
  int servo_base;
  struct position *next;
};

struct position *list = NULL, *last = NULL, *current = NULL;

void setup() {
  Serial.begin(9600);
  m_claw.attach(7);
  m_base.attach(6);
  m_base.write(pos_x);
  m_right.attach(5);
  m_right.write(pos_z);
  m_left.attach(4);
  pinMode(joystick, INPUT_PULLUP);
  pinMode(btn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(joystick), reset, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn), push_btn, FALLING);
  //digitalWrite(joystick, HIGH);
  //digitalWrite(btn, HIGH);
  pinMode(pin_x_key, INPUT);  
  pinMode(y_key_pin, INPUT); 
  lastSwitchDetectedMillis = millis();
  push_btn(); 
}

void loop() {
  //print_position();
  if (manual) {
    reading();
    time_delay = 15;
  }  
  else if (!pause) {
    play();
    time_delay = 1000;
  }
  delay(time_delay);  
}

void print_position() {
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
  Serial.print(" | joystick: ");
  Serial.print(digitalRead(joystick));
  Serial.println();
}

void reading() {
  pos_y = analogRead(pin_height);
  pos_y = map(pos_y, 0, 1023, 42, 180);
  claw = analogRead(pin_claw);
  claw = map(claw, 0, 1023, 0, 47);

  m_left.write(pos_y);
  m_claw.write(claw);

  if (analogRead(pin_x_key) < 490 && (pos_z + STEPS) <= 180 && (pos_z + STEPS) <= getMax(pos_y)) {
    pos_z += STEPS;
    //Serial.println(pos_z);
  }
  else if (analogRead(pin_x_key) > 510 && (pos_z - STEPS) >= getMin(pos_y)) {
    pos_z -= STEPS;
    //Serial.println(pos_z);
  }
  if (analogRead(y_key_pin) < 470) {
    pos_x -= STEPS;
    //Serial.println(pos_x);
  }
  else if (analogRead(y_key_pin) > 505) {
    pos_x += STEPS;
    //Serial.println(pos_x);
  }

  if (pos_y < 105) {
    pos_z = (pos_z < minDir(pos_y) ? minDir(pos_y) : pos_z);
  }

  if (pos_y > 135) {
    pos_z = (pos_z > maxDir(pos_y) ? maxDir(pos_y) : pos_z);
  }
  
  m_right.write(pos_z);
  m_base.write(pos_x);
}

void push_btn() {
  Serial.println("push_btn");
  if (millis() - lastSwitchDetectedMillis > debounceInterval) {
    lastSwitchDetectedMillis = millis();
    if (manual) {
      struct position *aux;
      aux = (struct position *)malloc(sizeof(struct position));
      if (!aux) return;
      aux -> servo_right = pos_z;
      aux -> servo_left = pos_y;
      aux -> servo_claw = claw;
      aux -> servo_base = pos_x;
      aux -> next = NULL;  

      if (!last) {
        list = last = aux;
      } else {
        last -> next = aux;
        last = last -> next;
      }
    }
    else {
      pause = !pause;
    }
  }
  return;    
}

void reset() {
  if (millis() - lastSwitchDetectedMillis > debounceInterval) {
    lastSwitchDetectedMillis = millis();
    pos_z = 120;
    pos_x = 80;  
    manual = !manual;
    //play();
    if (manual) clean();
  }
  return;
}

void play() {
  Serial.println("PLAY");
  if (!list) return;
  if (!current) current = list;

  Serial.println(current -> servo_base);
  m_right.write(current -> servo_right);
  m_base.write(current -> servo_base);
  m_left.write(current -> servo_left);
  m_claw.write(current -> servo_claw);

  current = current -> next;
  return;
}

void clean() {
  if (list) {
    struct position *walk, *prev;
    walk = list;
    while(walk) {
      prev = walk;
      walk = walk -> next;
      free(prev);
    }
    list = last = NULL;
  }
  return;
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
