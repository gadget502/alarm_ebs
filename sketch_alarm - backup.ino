#include <WebSocketClient.h>
#include <virtuabotixRTC.h>

#include<Adafruit_NeoPixel.h>
#include <TimeLib.h>
#include <WiFi.h>

#define PIN 2

// DS1302와 연결되는 아두이노 핀 번호
const int RST_PIN   = 9;   // Chip Enable : RST
const int IO_PIN   = 8;   // Input/Output : DAT
const int SCK_PIN = 7;   // Serial Clock : CLK

//네오픽셀을 사용하기 위해 객체 하나를 생성한다. 
//첫번째 인자값은 네오픽셀의 LED의 개수
//두번째 인자값은 네오픽셀이 연결된 아두이노의 핀번호
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, 25, NEO_GRB + NEO_KHZ800);

//  DS1302 오브젝트 생성
virtuabotixRTC myRTC(13, 5, 2);

// Constants
const char* ssid = "MisakaMOE";
const char* password = "ayh000729";

char path[] = "/socket.io/?EIO=3&transport=websocket";
char host[] = "192.168.0.17";

WiFiClient client;
WebSocketClient webSocketClient;

void setup() {
  Serial.begin(9600);
  //myRTC.setDS1302Time(00, 46, 20, 2, 26, 8, 2019);
//  Serial.println("Connecting...");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
//    Serial.print(".");
  }
  
   // Print our IP address
//  Serial.println("Connected!");
//  Serial.print("My IP address: ");
//  Serial.println(WiFi.localIP());
  
  if(client.connect("192.168.0.17",80)){
//      Serial.println("Connected");
    } else {
//      Serial.println("Connection failed");
    }
    
  webSocketClient.path=path;
  webSocketClient.host=host;

  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
  }

  strip.begin(); //네오픽셀을 초기화하기 위해 모든LED를 off시킨다
  strip.show();
}


void loop() {
  String data;

  // Le as informacoes do CI
  myRTC.updateTime(); 

  // Imprime as informacoes no serial monitor
  //Serial.print("Data : ");
  // Chama a rotina que imprime o dia da semana
//  imprime_dia_da_semana(myRTC.dayofweek);
//  Serial.print(", ");
//  Serial.print(myRTC.dayofmonth);
//  Serial.print("/");
//  Serial.print(myRTC.month);
//  Serial.print("/");
//  Serial.print(myRTC.year);
//  Serial.print("  ");
//  Serial.print("Time : ");
  // Adiciona um 0 caso o valor da hora seja <10
  if (myRTC.hours < 10)
  {
//    Serial.print("0");
  }
//  Serial.print(myRTC.hours);
//  Serial.print(":");
  // Adiciona um 0 caso o valor dos minutos seja <10
  if (myRTC.minutes < 10)
  {
//    Serial.print("0");
  }
//  Serial.print(myRTC.minutes);
//  Serial.print(":");
  // Adiciona um 0 caso o valor dos segundos seja <10
  if (myRTC.seconds < 10)
  {
//    Serial.print("0");
  }
//  Serial.println(myRTC.seconds);
 
  if (client.connected()) {
    webSocketClient.sendData("Info to be echoed back");
 
    webSocketClient.getData(data);
    if (data.length() > 0) {
//      Serial.print("Received data: ");
//      Serial.println(data);
    }
 
  } else {
//    Serial.println("Client disconnected.");
  }
 
  delay(1000);
  
  //아래의 순서대로 NeoPixel을 반복한다.  
  colorWipe(strip.Color(255, 0, 0), 50); //빨간색 출력
  //colorWipe(strip.Color(0, 255, 0), 50); //녹색 출력
  //colorWipe(strip.Color(0, 0, 255), 50); //파란색 출력

  //theaterChase(strip.Color(127, 127, 127), 50); //흰색 출력
  //theaterChase(strip.Color(127,   0,   0), 50); //빨간색 출력
  //theaterChase(strip.Color(  0,   0, 127), 50); //파란색 출력

  //화려하게 다양한 색 출력
  //rainbow(20);
  //rainbowCycle(20);
  //theaterChaseRainbow(50);
  //if(myRTC==alarmtime){}
  //알람을 실행 합니다.
  
}

void imprime_dia_da_semana(int dia)
{
  switch (dia)
  {
    case 1:
    Serial.print("Sunday");
    break;
    case 2:
    Serial.print("Monday");
    break;
    case 3:
    Serial.print("Tuesday");
    break;
    case 4:
    Serial.print("Wednesday");
    break;
    case 5:
    Serial.print("Thursday");
    break;
    case 6:
    Serial.print("Friday");
    break;
    case 7:
    Serial.print("Saturday");
    break;
  }
}




//NeoPixel에 달린 LED를 각각 주어진 인자값 색으로 채워나가는 함수
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

//모든 LED를 출력가능한 모든색으로 한번씩 보여주는 동작을 한번하는 함수
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//NeoPixel에 달린 LED를 각각 다른색으로 시작하여 다양한색으로 5번 반복한다
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { 
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//입력한 색으로 LED를 깜빡거리며 표현한다
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//LED를 다양한색으로 표현하며 깜빡거린다
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     //256가지의 색을 표현
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0); 
        }
    }
  }
}

//255가지의 색을 나타내는 함수
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
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
         Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte};  
  // Send the command line to DFPlayer  
  for (byte i=0; i<10; i++) Serial.write(Command_line[i]);  
}
void alarm_start(uint16_t vol){
  dpfExecute(0x06,vol,0x00); // VolumeSet
  dpfExecute(0x11,0x01,0x00); //repeatset
  dfpExecute(0x0F,0x01,0x01); //PlayMusic
}
void alarm_end(){
  dfpExecute(0x0E,0x00,0x00);//Pause(End)
}
boolean motion_check(){
    
  }
