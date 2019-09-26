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
const int pirPin = 39;
int pirStat = 0;
int move_cnt = 0;
int total_cnt = 0;
uint8_t alarm_time[4]= {25,61,61,8};
uint8_t alarm_LED[4]={255,255,255,10};
uint8_t alarm_DoW[7]={0,0,0,0,0,0,0};
int volume = 0x30;


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
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
//    Serial.print(".");
  pinMode(pirPin , INPUT); // pirPin 초기화
  }
  
// Print our IP address
// Serial.println("Connected!");
//  Serial.print("My IP address: ");
//  Serial.println(WiFi.localIP());
  
  if(client.connect("192.168.0.17",80)){
//      Serial.println("Connected");
    } else {
//      Serial.println("Connection failed");
    }
    
  webSocketClient.path=path;
  webSocketClient.host=host;

  strip.begin(); //네오픽셀을 초기화하기 위해 모든LED를 off시킨다
  strip.show();
}


void loop() {
  String data;
  String g_data;
  myRTC.updateTime(); 
 //if (!(myRTC.seconds % 10)){
    pirStat = digitalRead(pirPin); 
    if (pirStat == HIGH) {   // if motion detected
      move_cnt++;
        colorWipe(strip.Color(0 ,0 , 255), 10); 
      
    }
    else{
        colorWipe(strip.Color(0 ,255 , 0), 10);  
    }  
    total_cnt++;
   
    if((myRTC.minutes % 10 == 0) && (myRTC.seconds % 60 == 0)){
      data =String(move_cnt/total_cnt);
      //webSocketClient.sendData(data);
      move_cnt = 0;
      total_cnt = 0;
      }
    if((myRTC.hours==alarm_time[0])&&(myRTC.minutes==alarm_time[1])&&(myRTC.seconds==alarm_time[2])&&(alarm_DoW[myRTC.dayofweek-1]))//서버에서 받아오는 월처리
    {
      alarm_start(0x30);
      
    } 
    webSocketClient.getData(g_data);
    
    if (data.length() > 0) {
      if(1){
        //알람 설정
        alarm_set(data);
        }
      else if(1)
       alarm_end();
        //버튼을 이용한 알람 종료
      
    }
    else {
    }
 
  delay(1000);

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
void alarm_start(byte vol){
  dfpExecute(0x06,vol,0x00); // VolumeSet
  dfpExecute(0x11,0x01,0x00); //repeatset
  dfpExecute(0x0F,0x01,0x01); //PlayMusic
}
void alarm_end(){
  dfpExecute(0x0E,0x00,0x00);//Pause(End)
  colorWipe(strip.Color(0 ,0 , 0), 0);
}
void alarm_set(String socket){
  //socket set
  
  }
