// Speaker
#define SPK       32
#define SPK_FREQ 2400
#define SPK_CHANNEL 0
#define SPK_RESOLUTION 8
// LED
//#define LED_GREEN      2
//#define LED_BLUE      15

//Cam bien anh sang
//#include <BH1750FVI.h>
// Create the Lightsensor instance
//BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);
// Mqtt
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#define WIFI_AP "DinhQuan"
#define WIFI_PASSWORD "31132013"
char Thingsboard_Server[] = "easylora.vn";
#define User_name "CJKuKCKBBvxL9WFmWHE3"
#define Relay1 2 // IO2
#define Relay2 15 //IO4
//String rev = "command":"123";
//#define Relay3 14 // LED
WiFiClient wifiClient;
int status = WL_IDLE_STATUS;
PubSubClient client(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;
boolean gpioStatus[] = {false,false,false};
uint16_t gpioStatus_s[3];
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //LightSensor.begin();  // Khoi tao cam bien BH1750
  delay(10);
  pinMode(Relay1,OUTPUT);
  pinMode(Relay2,OUTPUT);
  //pinMode(Relay3,OUTPUT);
  digitalWrite(Relay1,HIGH);
  digitalWrite(Relay2,HIGH);
  //digitalWrite(Relay3,LOW);
  setupSpeaker();
  InitWiFi();
  client.setServer(Thingsboard_Server,1883);
  client.setCallback(callback);

}
 
void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
        reconnect();
    }
    client.loop();
    long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    client.publish("v1/devices/me/telemetry", get_sensor_status().c_str());
  }
  Serial.println(getChipID());
  delay(200);
  Serial.println(getShortChipID());
  delay(200);
}

void callback(const char* topic, byte* payload, unsigned int length){
    //Serial.println("On message");
    char json[length + 1];
    strncpy (json, (char*)payload, length);
    json[length] = '\0';
  
    Serial.println("Topic: ");
    Serial.println(topic);
    
    Serial.println("Message: ");
    
    Serial.println(json);
    
    StaticJsonBuffer<200> jsonBuffer;
    
    JsonObject  &data = jsonBuffer.parseObject((char*)json);
    
    if (!data.success()){
    Serial.println("parseObject() failed");
    return;
    }
    String methodName = String((const char*)data["method"]);
    if(methodName.equals("R1")){
      if(data["params"]==true){
          digitalWrite(Relay1,LOW);
          gpioStatus[0]=true;
          }
      else{
          digitalWrite(Relay1,HIGH);
          gpioStatus[0]=false;
          }
    }
/**************************************************/
    if(methodName.equals("R2")){
      if(data["params"]==true){
          digitalWrite(Relay2,LOW);
          gpioStatus[1]=true;
          }
      else{
          digitalWrite(Relay2,HIGH);
          gpioStatus[1]=false;
          }
    }
/**************************************************/
    if(methodName.equals("R3")){
      if(data["params"]==true){
          onSpeaker();
          //digitalWrite(Relay3,HIGH);
          gpioStatus[2]=true;
          }
      else{
          offSpeaker();
          //digitalWrite(Relay3,LOW);
          gpioStatus[2]=false;
          }
    }
    if(methodName.equals("sensor")){
      int pwm = data["params"];
      Serial.print("pwm = ");
      Serial.println(pwm);
      //analogWrite(1, pwm);
    }
    if(methodName.equals("setP1")){
      if(data["params"]==true){
          onSpeaker();
          //digitalWrite(Relay3,HIGH);
          gpioStatus[2]=true;
          }
      else{
          offSpeaker();
          //digitalWrite(Relay3,LOW);
          gpioStatus[2]=false;
          }
    }
    if(methodName.equals("config")){
      if(data["params"]=="on"){
          onSpeaker();
          //digitalWrite(Relay3,HIGH);
          gpioStatus[2]=true;
          delay(2000);
          offSpeaker();
          ESP.restart();
          }
      else{
          offSpeaker();
          //digitalWrite(Relay3,LOW);
          gpioStatus[2]=false;
          }
    }
    
/**************************************************/
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
    
  }
void reconnect(){
// Loop until we're reconnected
  while (!client.connected()){
    status = WiFi.status();
    if( status != WL_CONNECTED){
        WiFi.begin(WIFI_AP, WIFI_PASSWORD);
        while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
      }
    Serial.println("Connected to AP");
    }
  Serial.print("Connecting to Thingsboard node ...");
// Attempt to connect (clientId, username, password)
  if(client.connect("EASY LORA",User_name,NULL)){
    Serial.println( "[DONE]" );
    client.subscribe("v1/devices/me/rpc/request/+");//dang ky nhan cas lenh rpc tu cloud
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
    
    } 
  else{
    Serial.print( "[FAILED] [ rc = " );
    Serial.print( client.state());
    Serial.println(" : retrying in 5 seconds]");
    // Wait 5 seconds before retrying
    delay( 5000 );
    }
  }
}
String get_gpio_status(){
  // Prepare gpios JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  
  JsonObject& data = jsonBuffer.createObject();
  
  data["R1"]=gpioStatus[0] ? true : false;
  data["R2"]=gpioStatus[1] ? true : false;
  data["R3"]=gpioStatus[2] ? true : false;
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);
  Serial.println("Get gpio status: ");
  Serial.println(strPayload);
  return strPayload;
}
String get_sensor_status(){
  // Prepare gpios JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  
  JsonObject& data = jsonBuffer.createObject();
  //uint16_t lux = LightSensor.GetLightIntensity();
  data["sensor"]=random(30,50);
  data["sensor1"]=random(30,50);
  data["sensor2"]=random(30,50);
  char payload_s[256];
  data.printTo(payload_s, sizeof(payload_s));
  String strPayload_s = String(payload_s);
  Serial.println("Get sensor status: ");
  Serial.println(strPayload_s);
  return strPayload_s;
}
void InitWiFi(){
  Serial.println("Connecting to AP ...");
// attempt to connect to WiFi network
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    }
  Serial.println("Connected to AP");
}
