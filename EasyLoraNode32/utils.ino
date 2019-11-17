void setupSpeaker()
{
  // speaker
  ledcSetup(SPK_CHANNEL, SPK_FREQ, SPK_RESOLUTION);
  ledcAttachPin(SPK, SPK_CHANNEL);   
  onSpeaker();
  delay(100);
  offSpeaker(); 
}

void onSpeaker()
{
  Serial.println("On Speaker");
  //ledcWriteTone(SPK_CHANNEL, 2400);
  ledcWrite(SPK_CHANNEL, SPK_FREQ);
}

void offSpeaker()
{
  Serial.println("Off Speaker");
  ledcWrite(SPK_CHANNEL, 0);
}

// To get chip ID, ESP32 will return last 3 octets of mac address
String getShortChipID()
{  
  // Get default mac address
  uint8_t chipid[6];
  esp_efuse_mac_get_default (chipid);

  // Format into uppercase mac address without :
  // Eg: BCDDC2C31684
  char returnStr[9];
  snprintf(returnStr, 9, "%02X%02X%02X", chipid[3], chipid[4], chipid[5]);
  return returnStr;
}
// To get chip ID, ESP32 will return mac address
String getChipID()
{
  // Get default mac address
  uint8_t chipid[6];
  esp_efuse_mac_get_default (chipid);

  // Format into uppercase mac address without :
  // Eg: BCDDC2C31684
  char returnStr[15];
  snprintf(returnStr, 15, "%02X%02X%02X%02X%02X%02X",chipid[0], chipid[1], chipid[2], 
            chipid[3], chipid[4], chipid[5]);
  return returnStr;
}
void setupSpeaker()
{
  // speaker
  ledcSetup(SPK_CHANNEL, SPK_FREQ, SPK_RESOLUTION);
  ledcAttachPin(SPK, SPK_CHANNEL);   
  onSpeaker();
  delay(100);
  offSpeaker(); 
}

void onSpeaker()
{
  Serial.println("On Speaker");
  //ledcWriteTone(SPK_CHANNEL, 2400);
  ledcWrite(SPK_CHANNEL, SPK_FREQ);
}

void offSpeaker()
{
  Serial.println("Off Speaker");
  ledcWrite(SPK_CHANNEL, 0);
}
