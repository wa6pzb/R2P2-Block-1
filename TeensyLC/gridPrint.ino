/*

   9/09/2019 - 025: static declaration of battvc[]

 */


char *battVoltage() {
  int vread;
  float volt;
  int battv;
  static char battvc[12];
  analogReadAveraging(32);
  vread = analogRead(1);
  //Serial.println(vread);  // raw reading
  volt = (3.3 * vread)/1024; // 3.3 is the internal reference
  battv = volt * 200;        // no decimal points
  //convert to char array
  String volt_str = String(battv, DEC);
  volt_str.toCharArray(battvc, 4);
  Serial.print( " BATT=");
  Serial.println(battvc);
  return battvc;
}
