/*

   9/09/2019 - 025: static declaration of alt_c2[]
 */



char *altitude() {
  int gps_alt;
  char alt_c[6];
  static char alt_c2[6];
  // get altitude
  gps_alt = (gps.altitude() / 100);
  //Serial.println(gps_alt); // raw altitude
  //convert to char array
  String alt_str = String(gps_alt, DEC);
  alt_str.toCharArray(alt_c, 6);
  switch (gps_alt) {
    case -9999 ... 0:
      strcpy(alt_c2, "00000"); // negative or zero altitude ALL BALLS
      break;    
    case 1 ... 9:
      strcpy(alt_c2, "0000");
      strcat(alt_c2, alt_c);
      break;
    case 10 ... 99:
      strcpy(alt_c2, "000");
      strcat(alt_c2, alt_c);
      break;
    case 100 ... 999:
      strcpy(alt_c2, "00");
      strcat(alt_c2, alt_c);
      break;
    case 1000 ... 9999:
      strcpy(alt_c2, "0");
      strcat(alt_c2, alt_c);
      break;  
    case 10000 ... 99999:
      strcat(alt_c2, alt_c);
      break;          
  }
  //Serial.println(alt_c); //unpadded char array
  Serial.print(" ALT=");
  Serial.println(alt_c2);
  alt_c2[5] = '\0'; 
  return alt_c2;
}
