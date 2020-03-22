#include <AccelStepper.h>

AccelStepper stepper(1, A0, A1); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

void setup()
{  
    pinMode(38, OUTPUT);
    digitalWrite(38, LOW);   // turn the LED on (HIGH is the voltage level)
    stepper.setMaxSpeed(10000);
    stepper.setAcceleration(1000);

}

void loop()
{  
    for(int i=0; i<2; i++){
        stepper.moveTo(-500);
        stepper.runToPosition();
        stepper.moveTo(500);
        stepper.runToPosition();
    }
    delay(3000);
}
