// JTEncode call to loop through the string, transmitting one character at a time.
void encode()
{
  uint8_t i;

  // Reset the tone to the base frequency and turn on the output
  si5351.output_enable(SI5351_CLK0, 1);
  digitalWrite(LED_PIN, HIGH);

  // Now transmit the channel symbols

  for (i = 0; i < symbol_count; i++)
  {
    unsigned long starttime = micros(); // Get Start time - KJ6FO Fix
  si5351.set_freq((freq * 100) + (tx_buffer[i] * tone_spacing), SI5351_CLK0);
  while ((micros() - starttime) < 159000)
      {
      }; // Wait until time is up
  }

  // Turn off the output
  si5351.output_enable(SI5351_CLK0, 0);
  digitalWrite(LED_PIN, LOW);
}

void set_tx_buffer()
{
  // Clear out the transmit buffer
  memset(tx_buffer, 0, 255);


  jtencode.ft8_encode(message, tx_buffer);


}
