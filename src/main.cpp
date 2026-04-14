#include "config.h"
#include "rover.h"

Rover rover;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  Serial.println("Tracked Rover starting...");
  rover.begin();
}

void loop() {
  rover.update();
  delay(10);
}
