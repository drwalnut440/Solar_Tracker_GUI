#include <LDRTracker.h>
// Khởi tạo: chân servo + chân LDR
LDRTracker tracker(2, 13, A0, A3, A1, A2, A5);

void setup() {
  tracker.begin();
}

void loop() {
  tracker.update();
  float temp_val = tracker.getTempVal();
  int hor = tracker.getServoHori();
  int ver = tracker.getServoVert();

  // Serial.print("TEMP:");
  Serial.print(temp_val, 2);
  // Serial.print(",HORI:");
  Serial.print(hor);
  // Serial.print(",VERT:");
  Serial.println(ver);
}

