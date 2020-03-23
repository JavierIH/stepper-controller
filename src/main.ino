#include <AccelStepper.h>
#include <SPI.h>
#include <U8g2lib.h>


//Parameters
//const int motion_period = 2000;     //period of the motion
const int motion_amp = 5000;       //amplitude of the motion in steps
const float ie_ratio = 0.5;         //ratio between inspiration and expiration
const int n_cycles = 10;            //number of cycles before a delay
const int delay_time = 5000;        //delay between cycles, in ms
const int max_accel = 2000;         //maximum acceleration
const int max_speed = 3000;        //maximum speed

//RAMPS pinout
AccelStepper stepper(1, A0, A1);
U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, 23, 17, 16);

void setup(){  
    u8g2.begin();
    pinMode(38, OUTPUT);        //driver enable
    digitalWrite(38, LOW);
    stepper.setMaxSpeed(max_speed);
    stepper.setAcceleration(max_accel);
}

void loop(){  
    u8g2.firstPage();
    do {
        // u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.setFont(u8g2_font_victoriabold8_8u);
        u8g2.drawStr(0,10,"FRECUENCIA:  123");
        u8g2.drawStr(0,20,"AMPLITUD:    456");
        u8g2.drawStr(0,30,"PRESION:     789");
    } while ( u8g2.nextPage() );

    for(int i=0; i<n_cycles; i++){
        //inspiration
        stepper.setMaxSpeed(max_speed);
        stepper.moveTo(-motion_amp);
        stepper.runToPosition();

        //expiration
        stepper.setMaxSpeed(max_speed*ie_ratio);
        stepper.moveTo(0);
        stepper.runToPosition();
    }
    delay(delay_time);
}
