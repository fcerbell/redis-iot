static long lastADCTest = 0;
static long firstADCTest = 0;

MENUFCT(testADC) {
  Serial.println();
  Serial.println("Testing every second for 10 seconds...");
  firstADCTest = millis();
  CurrentState = ADC_func;
}

MENUFCT(ADC) {
  if ((millis() - lastADCTest) > 1000) {
    Serial.println(analogRead(0));
    lastADCTest = millis();
    return;
  }
  if ((millis() - firstADCTest) > 10000) {
    CurrentState = menu_func;
    return;
  }
}
