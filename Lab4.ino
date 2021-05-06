int button1 = 24;
int button2 = 25;
int button3 = 26;
int button4 = 27;
int buzzer_pin = 32;

const int number_in_bits[10] = {
  0b0000001,
  0b1001111,
  0b0010010,
  0b0000110,
  0b1001100,
  0b0100100,
  0b0100000,
  0b0001111,
  0b0000000,
  0b0000100
};

const int control_pins[8] = {
  13, 12, 11, 10, 50, 51, 52, 53
};

const int digit_pins[6] = {
  21, 20, 19, 18, 17, 16
};

struct Time {
  unsigned char seconds;
  unsigned char minutes;
  unsigned char hours;
};

Time timers[2] = {{0, 0, 0}, {0, 0, 0}};

unsigned int current_timer = 1;
bool timer_1_started;
bool timer_1_stopped;
bool timer_2_started;
bool timer_2_stopped;
bool set_hours_allowed;
bool set_minutes_allowed;
int iteration_counter = 0;

void setup() {
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(buzzer_pin, OUTPUT);
  for (int i = 0; i < 9; i++) {
    pinMode(control_pins[i], OUTPUT);
  }
  for (int i = 0; i < 6; i++) {
    pinMode(digit_pins[i], OUTPUT);
  }

  noInterrupts();
  TCCR0A = (1 << WGM01);
  OCR0A = 62496; 
  TIMSK0 |= (1 << OCIE0A);
  interrupts();
  TCCR0B |= (1 << CS01);
  TCCR0B |= (1 << CS00);
}

void loop() {

  int b1 = digitalRead(button1);
  int b2 = digitalRead(button2);
  int b3 = digitalRead(button3);
  int b4 = digitalRead(button4);
  render();
  if (!b1 && digitalRead(button1)) {
    first_click();
  }
  else  if (!b3 && digitalRead(button3)) {
    third_click();
  }
  else  if (!b2 && digitalRead(button2)) {
    second_click();
  }
  else  if (!b4 && digitalRead(button4)) {
    fourth_click();
  }
  if (current_timer) {
    if (timer_2_started && timer_2_stopped) {
      if (iteration_counter < 250) {
        digitalWrite(buzzer_pin, HIGH);
        delay(1000000);
        digitalWrite(buzzer_pin, LOW);
        timer_2_started = 0;
        timer_2_stopped = 0;
      }
      else {
        digitalWrite(buzzer_pin, LOW);
      }
    }
    else if (timer_1_started && timer_1_stopped) {
      if (iteration_counter < 250) {
        digitalWrite(buzzer_pin, HIGH);
        delay(1000000);
        digitalWrite(buzzer_pin, LOW);
        timer_1_started = 0;
        timer_1_stopped = 0;
      }
      else {
        digitalWrite(buzzer_pin, LOW);
      }
    }
  }
}


void render() {
  Time time_to_render = timers[current_timer];
  set_digit(5);
  show_number((int)time_to_render.seconds % 10);
  delay(1);
  set_digit(4);
  show_number((int) time_to_render.seconds / 10);
  delay(1);
  set_digit(3);
  show_number((int) time_to_render.minutes % 10);
  delay(1);
  set_digit(2);
  show_number( (int) time_to_render.minutes / 10);
  delay(1);
  set_digit(1);
  show_number((int) time_to_render.hours % 10);
  delay(1);
  set_digit(0);
  show_number( (int) time_to_render.hours / 10);
  delay(1);
}

void show_number(int number) {
  PORTB = 0b00000000;
  int bitmask = 0b0000001;
  int number_pinout = number_in_bits[number];
  int counter = 6;
  while (counter >= 0) {
    digitalWrite(control_pins[counter], bitmask & number_pinout);
    counter -= 1;
    bitmask <<= 1;
  }
}

void set_digit(int digit) {
  for (int pin = 0; pin < 6; ++ pin) {
    digitalWrite(digit_pins[pin], LOW);
  }
  digitalWrite(digit_pins[digit], HIGH);
}

void first_click() {
  if (set_hours_allowed) {
    if (timers[current_timer].hours < 99) {
      timers[current_timer].hours ++;
    }
    else {
      timers[current_timer].hours = 0;
    }
  }
  else {
    timers[current_timer].hours ++;
    set_hours_allowed = true;
    if (current_timer) {
      timer_2_started = false;
      timer_2_stopped = false;
    }
    else {
      timer_1_started = false;
      timer_1_stopped = false;
    }
  }
}

void second_click() {
  if (set_minutes_allowed) {
    if (timers[current_timer].minutes < 60) {
      timers[current_timer].minutes ++;
    }
    else {
      timers[current_timer].minutes = 0;
    }
  }
  else {
    set_minutes_allowed = true;
    timers[current_timer].minutes ++;
    if (current_timer) {
      timer_2_started = false;
      timer_2_stopped = false;
    }
    else {
      timer_1_started = false;
      timer_1_stopped = false;
    }
  }
}

void third_click() {
  current_timer = !current_timer;
}

void fourth_click() {
  if (current_timer) {
    if (timer_2_started) {
      timers[1] = {0, 0, 0};
      timer_2_started = false;
      timer_2_stopped = false;
    }
    else {
      timer_2_started = true;
      timer_2_stopped = false;
    }
  }
  else {
    if (timer_1_started) {
      timers[0] = {0, 0, 0};
      timer_1_started = false;
      timer_1_stopped = false;
    }
    else {
      timer_1_started = true;
      timer_1_stopped = false;
    }
  }
  set_hours_allowed = false;
  set_minutes_allowed = false;
}

void count_down_time() {
  if (timer_1_started) {

    if (timers[0].seconds > 0) {
      timers[0].seconds --;
    }
    else if (timers[0].minutes > 0) {
      timers[0].minutes--;
      timers[0].seconds = 59;
    }
    else if (timers[0].hours) {
      timers[0].hours -= 1;
      timers[0].minutes = 59;
      timers[0].seconds = 59;
    } else timer_1_stopped = true;


  }


  if (timer_2_started) {
    if (timers[1].seconds > 0) {
      timers[1].seconds --;
    }
    else if (timers[1].minutes > 0) {
      timers[1].minutes--;
      timers[1].seconds = 59;
    }
    else if (timers[1].hours) {
      timers[1].hours -= 1;
      timers[1].minutes = 59;
      timers[1].seconds = 59;
    }
    else timer_2_stopped = true;

  }
}


ISR(TIMER0_COMPA_vect) {
  if (iteration_counter == 500) {
    count_down_time();
    iteration_counter = 0;
  }
  else {
    iteration_counter++;
  }
}
