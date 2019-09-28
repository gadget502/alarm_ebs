#include <WebSocketClient.h>
#include<Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <string.h>
#include "DFRobotDFPlayerMini.h"

#define PIN 2

const int pirPin = 39;
int pirStat = 0;
float move_cnt = 0.0f;
float total_cnt = 0.0f;
uint8_t alarm_LED[3] = {255, 255, 255};
byte alarm_volume = 0x30;

boolean isAlarmStarted = false;

//네오픽셀을 사용하기 위해 객체 하나를 생성한다.
//첫번째 인자값은 네오픽셀의 LED의 개수
//두번째 인자값은 네오픽셀이 연결된 아두이노의 핀번호
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, 25, NEO_GRB + NEO_KHZ800);

// Constants
const char* ssid = "MisakaMOE";
const char* password = "ayh000729";

char path[] = "/socket.io/?EIO=3&transport=websocket";
char host[] = "192.168.0.14";

DFRobotDFPlayerMini myDFPlayer;

WiFiClient client;
WebSocketClient webSocketClient;

void setup() {
  //Initialize LED strip
  strip.begin(); //네오픽셀을 초기화하기 위해 모든LED를 off시킨다
  strip.show();
  //Initialize Serial
  Serial.begin(9600);
  //Initialize Wifi
  theaterChase(strip.Color(127, 127, 127), 5);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
  }
  pinMode(pirPin , INPUT); // pirPin 초기화
  theaterChase(strip.Color(255, 0, 127), 5);
  //Connect to Wifi
  client.connect("192.168.0.14", 3000);
  //Initialize WebSocket
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
} else {
}

  //Initialize MP3 Module
  theaterChase(strip.Color(0, 127, 127), 5);
  /*
  dfpExecute(0x3F, 0x00, 0x00);
  while(Serial.available()<10)
    delay(30);
    */
  if(!myDFPlayer.begin(Serial))
  {
    colorWipe(strip.Color(0,255,0),10);
    while(true);
  }
  myDFPlayer.volume(30);  //Set volume value (0~30).
  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  colorWipe(strip.Color(255,0,0),0);
  webSocketClient.sendData("setup");
}


void startAlarm(byte vol,uint8_t g,uint8_t r,uint8_t b) {
  colorWipe(strip.Color(g , r , b), 10);
  //dfpExecute(0x06, vol, 0x00); // VolumeSet
  myDFPlayer.play(1);  //Play the first mp3
  delay(1000);
    myDFPlayer.enableLoop(); //enable loop.
  delay(1000);
 
}

void setVolume(int vol)
{
  /*
    dfpExecute(0x06, 0x30, 0x00); // VolumeSet
    delay(30);
    */ 
    myDFPlayer.volume(vol);  //Set volume value (0~30).
    delay(1000);
}

void stopAlarm() {
      myDFPlayer.disableLoop(); //disable loop.
  delay(1000);
    myDFPlayer.pause();  //pause the mp3
  delay(1000);
  colorWipe(strip.Color(0 , 0 , 0), 0);
  isAlarmStarted = false;
}

void loop() {
   String data;
    webSocketClient.getData(data);
    if(data.length() > 0)
    {
      //Compare Header, then run proper methods for each header
      if(data == "LED")
      {
        String Red,Green,Blue;
        webSocketClient.getData(Red);
        webSocketClient.getData(Green);
        webSocketClient.getData(Blue);
   
        alarm_LED[0]= Green.toInt();
        alarm_LED[1]= Red.toInt();
        alarm_LED[2]= Blue.toInt(); 
        
      }
      else if(data == "AlarmStart"){
        startAlarm(alarm_volume,alarm_LED[0],alarm_LED[1],alarm_LED[2]);
        webSocketClient.sendData("Alarm Started");
      }
      else if(data == "AlarmStop"){
        stopAlarm();
        webSocketClient.sendData("Alarm Stopped");

      }
      else if (data == "CheckStart"){
        isAlarmStarted = true;
        webSocketClient.sendData("Checking started");

      }
       else if (data == "CheckStop"){
        isAlarmStarted = false;
      }
      else if(data == "MusicVolume"){
        String volume = "";
        webSocketClient.getData(volume);
        alarm_volume = volume.toInt();
        setVolume(alarm_volume);
        webSocketClient.sendData("Volume Changed to "+volume);

      }
    }
    else {
      //state check -> if alarm started, then check movement
      if(isAlarmStarted){
        recordMovement();
      }
    }
/*
    colorWipe(strip.Color(0 , 255 , 0), 10);
    //Client disconnected : Try to reconnect wi-fi
    while ( WiFi.status() != WL_CONNECTED ) {
      delay(500);
    }
   //Connect to Wifi
   client.connect("192.168.0.17", 80);//disconnected Visualize
 */
  
}


void recordMovement()
{
  pirStat = digitalRead(pirPin);
  if (pirStat == HIGH) {   // if motion detected
    move_cnt++;
    colorWipe(strip.Color(0 , 0 , 255), 10);
  }
  else {
    colorWipe(strip.Color(0 , 255 , 0), 10);
  }
  total_cnt++;

  if (total_cnt>=60) {
    webSocketClient.sendData(String((move_cnt*1.0) / total_cnt));
    move_cnt = 0.0;
    total_cnt = 0.0;
  }
  delay(910);
}

//NeoPixel에 달린 LED를 각각 주어진 인자값 색으로 채워나가는 함수
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

//모든 LED를 출력가능한 모든색으로 한번씩 보여주는 동작을 한번하는 함수
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//NeoPixel에 달린 LED를 각각 다른색으로 시작하여 다양한색으로 5번 반복한다
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//입력한 색으로 LED를 깜빡거리며 표현한다
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//LED를 다양한색으로 표현하며 깜빡거린다
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   //256가지의 색을 표현
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255));
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);
      }
    }
  }
}

//255가지의 색을 나타내는 함수
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void dfpExecute(byte CMD, byte Par1, byte Par2)
{
# define Start_Byte   0x7E
# define Version_Byte  0xFF
# define Command_Length 0x06
# define Acknowledge  0x00
# define End_Byte    0xEF
  // Calculate the checksum (2 bytes)
  uint16_t checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
  // Build the command line
  uint8_t Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
                               Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
                             };
  // Send the command line to DFPlayer
  for (byte i = 0; i < 10; i++) Serial.write(Command_line[i]);
}
