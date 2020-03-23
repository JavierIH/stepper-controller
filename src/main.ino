#include <AccelStepper.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <TimerOne.h>
#include <ClickEncoder.h>

//Parameters
//const int motion_period = 2000;     //period of the motion
int16_t motion_amp = 5000;          //amplitude of the motion in steps
float ie_ratio = 0.5;           //ratio between inspiration and expiration
uint16_t n_cycles = 1;               //number of cycles before a delay
uint16_t delay_time = 20;            //delay between cycles, in ms
uint16_t max_accel = 2000;           //maximum acceleration
uint16_t max_speed = 3000;           //maximum speed

uint8_t stepper_en = 38;
uint8_t stepper_step = A0;
uint8_t stepper_dir = A1;

uint8_t encoder_right_pin = 31;
uint8_t encoder_left_pin = 33;
uint8_t encoder_button_pin = 35;
int16_t enc_pos;
uint8_t buttonState;


AccelStepper stepper(1, stepper_step, stepper_dir);
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, 23, 17, 16);
ClickEncoder encoder(encoder_left_pin, encoder_right_pin, encoder_button_pin, 1);

void setup(){
    Serial.begin(9600);
    u8g2.begin();
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    encoder.setAccelerationEnabled(true);

    pinMode(stepper_en, OUTPUT);        //driver enable
    digitalWrite(stepper_en, LOW);
    stepper.setMaxSpeed(max_speed);
    stepper.setAcceleration(max_accel);
}

const char text_template[6][17] = {
    " AMP:           ",
    " IE_RATIO:      ",
    " DELAY:         ",
    " PARAM1:        ",
    " PARAM2:        ",
    " PARAM3:        "
};

uint8_t line_sel=0;
int16_t inc=1;
int64_t last_screen_update=-200;

void loop(){
    if(inc){
        motion_amp += inc*10;
        if(motion_amp < 0) motion_amp = 0;
        if(millis() - last_screen_update > 200){ //Refresh screen only once every 200ms
        char text [6][17];
        memcpy(text, text_template, sizeof(text_template));
        u8g2.firstPage();
        do{
            u8g2.setFont(u8g2_font_victoriabold8_8u);
            u8g2.drawStr(0,10,text_template[0]);
            //u8g2.setDrawColor(0);

            sprintf(text[0], " AMP: %d", motion_amp);
            for(int i=0; i < 6; i++){
                if(i == line_sel){
                    text[i][0] = '>';
                }
                u8g2.drawStr(0,10+i*10, text[i]);
            }

        }while ( u8g2.nextPage() );
        last_screen_update = millis();
        }
    }


    if (!stepper.isRunning()){
        if(stepper.currentPosition()==0){
            stepper.setMaxSpeed(max_speed);
            stepper.moveTo(motion_amp);
        }
        else{
            stepper.setMaxSpeed(max_speed*ie_ratio);
            stepper.moveTo(0);
        }
    }

    inc = encoder.getValue();
    stepper.run();
}

void timerIsr() {
    encoder.service();
}