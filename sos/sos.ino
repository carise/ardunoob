/**
 * My newbie SOS
 */

int led = 13;
int chars_printed = 0;
int next_long = 1;
int next_loop_pause = 1500;
int next_char_pause = 300;
int delay_wait_on = 900;
int delay_wait_off = 300;

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  if (next_long == 0) {
    // S
    delay_wait_on = 500;
    next_long = 1;
  } else {
    // O
    delay_wait_on = 1000;
    next_long = 0;
  }

  for (int i = 0; i < 3; i++) {
    digitalWrite(led, HIGH);
    delay(delay_wait_on);
    digitalWrite(led, LOW);
    delay(delay_wait_off);
  }
  chars_printed++;
  if (chars_printed == 3) {
    next_long = 0;
    chars_printed = 0;
    digitalWrite(led, LOW);
    delay(next_loop_pause);
  }
  delay(next_char_pause);
}
