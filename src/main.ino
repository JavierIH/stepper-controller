#include <AccelStepper.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <TimerOne.h>
#include <ClickEncoder.h>

//Parameters
int32_t motion_amp = 10000;         //amplitude of the motion in steps
float ie_ratio = 0.5;               //ratio between inspiration and expiration
int16_t n_cycles = 1;               //number of cycles before a delay
int32_t delay_time = 20;            //delay between cycles, in ms
int32_t max_accel = 50000;          //maximum acceleration
int32_t max_speed = 10000;          //maximum speed

//Pinout
uint8_t stepper_en = 38;
uint8_t stepper_step = A0;
uint8_t stepper_dir = A1;
uint8_t encoder_right_pin = 31;
uint8_t encoder_left_pin = 33;
uint8_t encoder_button_pin = 35;

//Devices
AccelStepper stepper(1, stepper_step, stepper_dir);
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, 23, 17, 16);
ClickEncoder encoder(encoder_left_pin, encoder_right_pin, encoder_button_pin, 4);

//State variables
int16_t enc_pos;
uint8_t button_state, old_button_state;
char text_buffer[6][17];
int8_t cursor_pos = 0;
int16_t encoder_inc = 1;
int64_t last_screen_update = -200;
boolean menu_mode = true;


void setup(){
    Serial.begin(9600);

    u8g2.begin();
    u8g2.setFont(u8g2_font_victoriabold8_8r);

    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    encoder.setAccelerationEnabled(true);

    pinMode(stepper_en, OUTPUT);        //driver enable
    digitalWrite(stepper_en, LOW);
    stepper.setMaxSpeed(max_speed);
    stepper.setAcceleration(max_accel);

    sprintf(text_buffer[0], " AMP: %d", motion_amp);
    sprintf(text_buffer[1], " IE_RATIO: %d.%d", (int)ie_ratio, (int)(ie_ratio*10)%10);
    sprintf(text_buffer[2], " SPEED: %d", max_speed);
    sprintf(text_buffer[3], " CYCLES: %d", n_cycles);
    sprintf(text_buffer[4], " DELAY: %d", delay_time);
    sprintf(text_buffer[5], " START/STOP: ");
}


void loop(){
    // Update user controls
    button_state = encoder.getButton();
    encoder_inc = encoder.getValue();

    if(encoder_inc || button_state != old_button_state){
        old_button_state = button_state;

        if(menu_mode){ // cursor mode, user can move the cursor up and down
            cursor_pos += encoder_inc;
            cursor_pos = constrain(cursor_pos, 0, 5);
        }
        else{ // set value mode (parameter selected)
            switch (cursor_pos){
            case 0:
                motion_amp += encoder_inc*10;
                if(motion_amp < 0) motion_amp = 0;
                sprintf(text_buffer[0], " AMP: %d", motion_amp);
                break;
            case 1:
                ie_ratio += encoder_inc*0.1;
                if(ie_ratio < 0.1) ie_ratio = 0.1;
                sprintf(text_buffer[1], " IE_RATIO: %d.%d", (int)ie_ratio, (int)(ie_ratio*10)%10);
                break;                
            case 2:
                max_speed += encoder_inc*10;
                if(max_speed < 0) max_speed = 0;
                sprintf(text_buffer[2], " SPEED: %d", max_speed);
                break;
            case 3:
                n_cycles += encoder_inc;
                if(n_cycles < 0) n_cycles = 0;
                sprintf(text_buffer[3], " CYCLES: %d", n_cycles);
                break;
            case 4:
                delay_time += encoder_inc*10;
                if(delay_time < 0) delay_time = 0;
                sprintf(text_buffer[4], " DELAY: %d", delay_time);
                break;
            case 5:
                sprintf(text_buffer[5], " START/STOP: ");
                break;
            default:
                break;
            }
        }

        if(button_state == ClickEncoder::Clicked){ //toggle from cursor mode to set value mode
            menu_mode = !menu_mode; 
        }

        if(millis() - last_screen_update > 50){ //Refresh screen only once every 200ms
            u8g2.firstPage();
            for(int i=0; i < 6; i++){
                u8g2.setDrawColor(1);
                if(i == cursor_pos){
                    text_buffer[i][0] = '>';
                    if(!menu_mode) u8g2.setDrawColor(0); // set value mode
                }
                else{
                    text_buffer[i][0] = ' ';
                }
                u8g2.drawStr(0,10+i*10, text_buffer[i]);
            }
        while (u8g2.nextPage());
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
    stepper.run();
}

void timerIsr() {
    encoder.service();
}