#include "arduino.h"

class DCMotor {
public:
    DCMotor(int enablePin, int in1Pin, int in2Pin)
            : enablePin_(enablePin), in1Pin_(in1Pin), in2Pin_(in2Pin) {
        // Set the pin modes for the enable, in1, and in2 pins
        pinMode(enablePin_, OUTPUT);
        pinMode(in1Pin_, OUTPUT);
        pinMode(in2Pin_, OUTPUT);

        // Set the initial state of the motor to stopped
        stop();
    }

    void start() {
        // Set the enable pin to high to enable the motor driver
        digitalWrite(enablePin_, HIGH);

        // Set the in1 and in2 pins to high and low, respectively, to
        // drive the motor in one direction
        digitalWrite(in1Pin_, HIGH);
        digitalWrite(in2Pin_, LOW);
    }

    void stop() {
        // Set the enable pin to low to disable the motor driver
        digitalWrite(enablePin_, LOW);

        // Set the in1 and in2 pins to low to ensure that the motor is not
        // being driven
        digitalWrite(in1Pin_, LOW);
        digitalWrite(in2Pin_, LOW);
    }

    void reverse() {
        // Set the enable pin to high to enable the motor driver
        digitalWrite(enablePin_, HIGH);

        // Set the in1 and in2 pins to low and high, respectively, to
        // drive the motor in the opposite direction
        digitalWrite(in1Pin_, LOW);
        digitalWrite(in2Pin_, HIGH);
    }

private:
    // The pin numbers for the enable, in1, and in2 pins
    int enablePin_;
    int in1Pin_;
    int in2Pin_;
};

class LightController {
public:
    // Switches state of LED to HIGH or LOW depending on current state.
    // id: The id of the LED.
    // delayTime: The number of milliseconds to wait before switching the LED.
    void switchLight(int id) {
        // Check if the LED is currently on or off.
        digitalRead(id) == HIGH ? digitalWrite(id, LOW) : digitalWrite(id, HIGH);
    }

    void turnLightOff(int id) {
        digitalWrite(id, LOW);
    }

    // Makes LED blink (n) amount of times.
    void blinkLight(int id) {
        // Loop until the specified number of cycles have been completed.
        // Switch the LED on or off.
        this->switchLight(id);
        delay(400);
        this->switchLight(id);
    }
};


// Create a DCMotor object using pin numbers 2, 3, and 4 for the enable, in1,
// and in2 pins, respectively
DCMotor motor(2, 3, 4);

LightController LC;

const int warningLEDLightPin = 5, upButtonPin = 6, downButtonPin = 7, pauseButtonPin = 8, emergencyStopButtonPin = 9;

unsigned long forwardStartTime = 0;
unsigned long reverseStartTime = 0;

// Define the possible states of the state machine.
enum State {
    STOPPED,
    FORWARD, // UP
    REVERSE // DOWN
} state = STOPPED;

void setup() {
    // Put your setup code here
    forwardStartTime = millis();
    reverseStartTime = millis();
}

void loop() {
    // Check the state of each button and take appropriate action.
    switch (state) {
        case STOPPED:
            // If the "UP" button is pressed, start the motor in the forward direction.
            if (digitalRead(upButtonPin) == HIGH) {
                motor.start();
                state = FORWARD;

                // Record the time that the motor starts running in the forward direction.
                forwardStartTime = millis();
            }
                // If the "DOWN" button is pressed, start the motor in the reverse direction.
            else if (digitalRead(downButtonPin) == HIGH) {
                motor.reverse();
                state = REVERSE;

                // Record the time that the motor starts running in the reverse direction.
                reverseStartTime = millis();
            }
            break;
        case FORWARD:
            // If the "STOP" button is pressed, stop the motor.
            if (digitalRead(pauseButtonPin) == HIGH) {
                motor.stop();
                state = STOPPED;
            }
                // If the "EMERGENCY STOP" button is pressed, stop the motor immediately.
            else if (digitalRead(emergencyStopButtonPin) == HIGH) {
                motor.stop();
                state = STOPPED;
                LC.blinkLight(warningLEDLightPin);
            } else {
                // Check how long the motor has been running in the forward direction.
                unsigned long currentTime = millis();
                unsigned long elapsedTime = currentTime - forwardStartTime;

                if (elapsedTime >= 4000) {
                    // The motor has been running for at least 4 seconds, so we can stop it.
                    motor.stop();
                    state = STOPPED;
                }
            }
            break;
        case REVERSE:
            // If the "STOP" button is pressed, stop the motor.
            if (digitalRead(pauseButtonPin) == HIGH) {
                motor.stop();
                state = STOPPED;
            }
                // If the "EMERGENCY STOP" button is pressed, stop the motor immediately.
            else if (digitalRead(emergencyStopButtonPin) == HIGH) {
                motor.stop();
                state = STOPPED;
                LC.blinkLight(warningLEDLightPin);
            } else {
                // Check how long the motor has been running in the reverse direction.
                unsigned long currentTime = millis();
                unsigned long elapsedTime = currentTime - reverseStartTime;

                if (elapsedTime >= 4000) {
                    // The motor has been running for at least 4 seconds, so we can stop it.
                    motor.stop();
                    state = STOPPED;
                }
            }
            break;
    }
}
