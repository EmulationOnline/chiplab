// 6502 driver for chiplab
// See more at https://emulationonline.com/systems/chiplab/
//
// All commands should respond with a newline terminated string.
// Current commands (only first letter necessary):
// (s)tep - toggles the clock then returns the output pins
// (r)eset - sets reset low and toggles the clock for several cycles to initialize
//           the chip.
// d=DECIMAL - set the data bus to the given decimal value. This is an 8 bit bus, so
//             valid values are 0 through 255.
// (t)est - read the buses off the chip without changing other state.

// Shift registers used for address bus
const int SHIFT_ADDR_LOW_BIT = 5; // PC6;
const int SHIFT_ADDR_HIGH_BIT = 13; // PC7;
const int SHIFT_SHIFT_ELSE_LOAD = A5; // PF0;  // high shifts, low loads. ignores clock.
const int SHIFT_CLOCK = A4; // PF1;
const int MOS_PIN_SYNC = 9; // PB5

// irq pins, outputs but should be set not not interrupt at
// startup
const int MOS_PIN_NMIB = 11; // PB7
const int MOS_PIN_IRQB = 7; // PE6
// MLB is an input to track read-modify-write instructions. unused for now.
const int MOS_PIN_MLB = 10; // PB6

// Output to MOS
// PE2 aka HWB Doesnt have a normal digital pin number. Write directly.
// const int MOS_PIN_RESET = PE2;
void setup_mos_pin_reset() {
  DDRE |= 0x04;  // bit 2 output  
}
void set_mos_pin_reset(bool v) {
  if (v) {
    PORTE |= 0x04;
  } else {
    PORTE &= ~0x04;
  }
}
const int MOS_PIN_CLOCK = 17; // PB0;
// Input from MOS
const int MOS_PIN_RWB = 8; // PB4;

// RW data pins
const int DATA_PINS_HL [] = {
  // high to low, since this is natural order for reading
  6,  // PD7,
  12, // PD6,
  30, // PD5,
  4,  // PD4,
  
  1,  // PD3,
  0,  // PD2,
  2,  // PD1,
  3,  // PD0,
};

void data_mode(int mode) {
  static_assert(sizeof(DATA_PINS_HL) / sizeof(int) == 8);
  for (int i = 0; i < 8; i++) {
    pinMode(DATA_PINS_HL[i], mode);
  }
}
void data_set(uint8_t value) {
  // set pins to output
  data_mode(OUTPUT);
  for (int i = 0; i < 8; i++) {
    bool v = (value>>i) & 1;
    
    digitalWrite(DATA_PINS_HL[7-i], v);
  }
  hold_6502();
}

void setup() {
  pinMode(SHIFT_ADDR_LOW_BIT, INPUT);
  pinMode(SHIFT_ADDR_HIGH_BIT, INPUT);
  pinMode(SHIFT_SHIFT_ELSE_LOAD, OUTPUT);
  pinMode(SHIFT_CLOCK, OUTPUT);

  pinMode(MOS_PIN_RWB, INPUT);
  pinMode(MOS_PIN_MLB, INPUT);
  pinMode(MOS_PIN_CLOCK, OUTPUT);
  pinMode(MOS_PIN_IRQB, OUTPUT); digitalWrite(MOS_PIN_IRQB, 1);
  pinMode(MOS_PIN_NMIB, OUTPUT); digitalWrite(MOS_PIN_NMIB, 1);
  setup_mos_pin_reset(); // pinMode(MOS_PIN_RESET, OUTPUT);
  data_mode(INPUT);


  Serial.begin(9600);
}

char line[1024];
void loop() {
  size_t n = Serial.readBytesUntil('\n', line, 1022);
  if (n == 0) return;
  line[n] = '\n';
  line[n+1] = 0;

    const char* resp = nullptr;
    switch(line[0]) {
      case 'i': { resp = "6502_0.1"; }; break;
      case 'r' : {
        reset_6502();
        shift_reset();
        // implicit output in above.
      }; break;
      case 's': {
        step_6502();
        // response is implicit in above
      }; break;
      case 'd': {
        // d1234578\n
        // sets the data bus
        data_6502(line);
        // io implicit
      } break;
      case 't' : {
        // status without step
          status_6502();
          // response implicit
      } break;
      default: { resp = "unknown"; } break;
    }
    if (resp != nullptr) {
      Serial.println(resp);
    }
}

void old_main() {
  Serial.println("pins setup");

  reset_6502();
  shift_reset();
  status_6502();
  data_set(0x7A);
  for (int i = 0; i < 20; i++) {
    step_6502();
  }
}

void data_6502(const String& line) {
  bool found = false;
  int32_t acc = 0;
  for(int i = 1; i < line.length(); i++) {
    if (line[i] == '\n') {
      found = true;
    } else {
      char c = line[i];
      if (c >= '0' && c <= '9') {
        acc = acc*10 + (c - '0');
      } else {
        Serial.print("bad digit:");
        Serial.println(c);
        return;
      }
    }
  }
  if (!found) {
    Serial.println("no newline, skipping data");
  } else {
    // set data bits
    acc = acc & 0xFF;
    data_set(acc);
    Serial.print("d=0x");
    Serial.println(acc, HEX);

  }
}

void shift_reset() {
  digitalWrite(SHIFT_SHIFT_ELSE_LOAD, 0);
  digitalWrite(SHIFT_CLOCK, 0);
  shift_delay();
}

void shift_delay() {
//   delayMicroseconds(1000);
  delayMicroseconds(1000*10);
}
void shift_load() {
  digitalWrite(SHIFT_SHIFT_ELSE_LOAD, 1);
  shift_delay();  
  digitalWrite(SHIFT_SHIFT_ELSE_LOAD, 0);
  shift_delay();
  digitalWrite(SHIFT_SHIFT_ELSE_LOAD, 1);
  shift_delay();  
}
void shift_shift() {  
  digitalWrite(SHIFT_CLOCK, 0);
  shift_delay();
  digitalWrite(SHIFT_CLOCK, 1);
  shift_delay();
}

uint16_t read_addr() {
  // address comes from shift register.
  // currently loads low 8 bits only.  
  shift_load();
  
  uint16_t low = 0;
  uint16_t high = 0;
  for (int p = 0; p < 8; p++) {
    low <<= 1;
    low |= digitalRead(SHIFT_ADDR_LOW_BIT);
    high <<= 1;
    high |= digitalRead(SHIFT_ADDR_HIGH_BIT);
    shift_shift();
  }

  return (high << 8) | low;  
}

uint8_t read_data() {
  // data is set by chip whenever rwb says write.
  // otherwise its a read and these pins are inputs on the 6502.
  data_mode(INPUT);
  uint8_t data = 0;
  // TODO: double check, changed with pin reassignment
  
  for (int p = 0; p < 8; p++) {
    data <<= 1;
    data |= digitalRead(DATA_PINS_HL[p]);
  }
  return data;
}

void status_6502() {
  uint16_t addr = read_addr();
  
  bool is_read = digitalRead(MOS_PIN_RWB);
  Serial.print("a=0x");
  Serial.print(addr, HEX);
  Serial.print(" r=");
  Serial.print(is_read);
  if (!is_read) {
    Serial.print(" d=0x");
    Serial.print(read_data(), HEX);
  }
  bool is_sync = digitalRead(MOS_PIN_SYNC);
  Serial.print(" sync=");
  Serial.print(is_sync);
  Serial.print("\n");
}

void step_6502() {
  // toggle clock, read new state
  clock_6502();
  status_6502();
}

void hold_6502() {
  // 1ms hold time = 1k transitions / sec -> 500hz
  delayMicroseconds(1000);
  //status_6502();
}

void clock_6502() {
  hold_6502();
  digitalWrite(MOS_PIN_CLOCK, 0);
  hold_6502();
  digitalWrite(MOS_PIN_CLOCK, 1);
}

void reset_6502() {
  set_mos_pin_reset(0);
  for (int i = 0; i < 5; i++) {
    clock_6502();
  }

  set_mos_pin_reset(1);
  Serial.println("reset.");
}
