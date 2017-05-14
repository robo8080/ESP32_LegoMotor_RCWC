#include <WiFi.h>
#include <WiFiUdp.h>
#include "RCWController.h"
// This is the library for the TB6612 that contains the class Motor and all the
// functions
#include "ESP32_SparkFun_TB6612.h"

const char ssid[] = "ESP32";  //  your network SSID (name)
const char pass[] = "esp32pass";  // your network password

WiFiUDP udp;
unsigned int localPort = 10000;
const int PACKET_SIZE = 256;
char packetBuffer[PACKET_SIZE];

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// the default pins listed are the ones used on the Redbot (ROB-12097) with
// the exception of STBY which the Redbot controls with a physical switch
#define AIN1 27
#define BIN1 14
#define AIN2 26
#define BIN2 12
#define PWMA 25
#define PWMB 13
#define STBY 16
// use first channel of 16 channels (started from zero)
#define CHA 0
#define CHB 1

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, CHA);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, CHB);

void setup()
{
  Serial.begin(115200);

  WiFi.softAP(ssid, pass);
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(myIP);

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(localPort);
  motor1.brake();
  motor2.brake();
}

void loop() {
  int rlen;

  while (1) {
    rlen = udp.parsePacket();
    if(rlen<10) {
      delay(10);
      continue;
    }
    udp.read(packetBuffer, (rlen > PACKET_SIZE) ? PACKET_SIZE : rlen);
    memcpy( &controller.data[0], packetBuffer, sizeof(controller)); 
//    Serial.printf("DATA:%02X %02X %d %d %d %d %d %d %d %02X\n\r",controller.data[0],controller.data[1],controller.data[2],controller.data[3],controller.data[4],
//                                                               controller.data[5],controller.data[6],controller.data[7],controller.data[8],controller.data[9]);
    Serial.printf("Left  Analog UD : %d\n\r",controller.status.LeftAnalogUD);
    Serial.printf("Right Analog LR : %d\n\r",controller.status.RightAnalogLR);
    int speed1 = ((int)controller.status.LeftAnalogUD-128)*2;
    int speed2 = ((int)controller.status.RightAnalogLR-128)*2;
    Serial.printf("Speed1 = %d Speed2 = %d\n\r",speed1,speed2);
    motor1.drive(speed1);
    motor2.drive(speed2);
   }
}

