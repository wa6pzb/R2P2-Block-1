char *battVoltage() {
  int battv;
  char battvc[12];
  analogReadAveraging(32);
  battv = analogRead(1);
  Serial.println(battv);
  //convert to char array
  String volt_str = String(battv, DEC);
  volt_str.toCharArray(battvc, 4);
  Serial.println(battvc);
  return battvc;
}
