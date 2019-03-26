char *gridPrint() {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());

    double lat, lon, lat1, lon1;                                 // GPS variables
    long lat2, lon2, lat3, lon3, lat4, lon4, lat5, lon5, lat6, lon6, lat7, lon7;
    
    char mh[12];

    // step 1 convert to milliseconds
    lat1 = flat * 3600000L;                              // 3600000 milliseconds (ms) per degree
    lon1 = flon * 3600000L;

    // Step 2 normalize for Maidenhead coordinates
    lon2 = lon1 + 648000000L;                            // add to 180 degrees in ms to Lon
    lat2 = lat1 + 324000000L;                            // add tp 90 degrees in ms to Lat

    // step 3 calculate 1st grid pair into character array and get remainder
    mh[0] = 'A' + int(lon2 / 72000000L);                 // Lon / 20 degrees in ms
    mh[1] = 'A' + int(lat2 / 36000000L);                 // Lat / 10 degrees in ms
    lon3 = (lon2 % 72000000L);                           // get the remainder for next step
    lat3 = (lat2 % 36000000L);                           // get the remainder for next step

    // step 4 calculate 2nd grid pair into character array and get remainder
    mh[2] = '0' + int(lon3 / 7200000L);                 // Lon / 2 degrees in ms
    mh[3] = '0' + int(lat3 / 3600000L);                 // Lat / 1 degrees in ms
    lon4 = (lon3 % 7200000L);                           // get the remainder for next step
    lat4 = (lat3 % 3600000L);                           // get the remainder for next step

    // step 5 calculate 3rd grid pair into character array and get remainder
    mh[4] = 'A' + int(lon4 / 300000L);                 // Lon / .083333 degrees in ms
    mh[5] = 'A' + int(lat4 / 150000L);                 // Lat / .041666 degrees in ms
    lon5 = (lon4 % 300000L);                           // get the remainder for next step
    lat5 = (lat4 % 150000L);                           // get the remainder for next step

    // step 6 calculate 4th grid pair into character array and get remainder
    mh[6] = '0' + int(lon5 / 30000L);                 // Lon / .0083333 degrees in ms
    mh[7] = '0' + int(lat5 / 15000L);                 // Lat / .0041666 degrees in ms
    lon6 = (lon5 % 30000L);                           // get the remainder for next step
    lat6 = (lat5 % 15000L);                           // get the remainder for next step

    // step 7 calculate 5th grid pair into character array and get remainder
    mh[8] = 'A' + int(lon6 / 1250L);                 // Lon / .0003472 degrees in ms
    mh[9] = 'A' + int(lat6 / 625L);                 // Lat / .0001736 degrees in ms
    lon7 = (lon6 % 1250L);                           // get the remainder for next step
    lat7 = (lat6 % 625L);                           // get the remainder for next step


    mh[10] = '\0';                                       // end character array
    Serial.print(" GRID=");
    Serial.println(mh);
  
  return mh;
}
