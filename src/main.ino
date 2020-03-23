#include <AccelStepper.h>
#include <SPI.h>
#include <U8g2lib.h>

AccelStepper stepper(1, A0, A1); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
//U8G2_UC1701_DOGS102_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_ST7920_128X64_2_HW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, 23, 17, 16);
void setup()
{  
      u8g2.begin();
    pinMode(38, OUTPUT);
    digitalWrite(38, LOW);   // turn the LED on (HIGH is the voltage level)
    stepper.setMaxSpeed(10000);
    stepper.setAcceleration(2000);

}

void loop()
{  
     u8g2.firstPage();
  do {
   // u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setFont(u8g2_font_victoriabold8_8u);
    u8g2.drawStr(0,10,"FRECUENCIA:  123");
    u8g2.drawStr(0,20,"AMPLITUD:    456");
    u8g2.drawStr(0,30,"PRESION:     789");
  } while ( u8g2.nextPage() );
    for(int i=0; i<2; i++){
        stepper.moveTo(-1000);
        stepper.runToPosition();
        stepper.moveTo(1000);
        stepper.runToPosition();
    }
    //delay(3000);
}
