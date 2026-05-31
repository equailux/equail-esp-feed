#include "sequence.h"
#include "./limitter.h"
#include "./motor.h"
#include "./servo.h"
#include <Arduino.h>

//

namespace fed::internal
{
    enum class Step
    {
        Idle,
        OpenServo1,
        WaitServo1,
        CloseServo1,
        StartMotorForward,
        WaitMotorForward,
        StopMotorForward,
        OpenServo2,
        WaitServo2,
        CloseServo2,
        StartMotorReverse,
        WaitMotorReverse,
        StopMotorReverse,
    };

    // writeServo uses 181+ to address servo 2 and applies state % 180.
    const int servo1OpenInput = 180;
    const int servo1CloseInput = 0;
    const int servo2OpenInput = 359;
    const int servo2CloseInput = 360;

    Step step = Step::Idle;
    Command command = {5000, 5000, 10000};
    unsigned long stepStartedAt = 0;
    bool motorStoppedByLimitter = false;

    //

    void logStepEntry(Step current)
    {
        switch (current)
        {
        case Step::Idle:
            Serial.printf("Feeding automation step: Idle.\n");
            break;
        case Step::OpenServo1:
            Serial.printf("Feeding automation step: OpenServo1 (opening servo 1).\n");
            break;
        case Step::WaitServo1:
            Serial.printf("Feeding automation step: WaitServo1 (waiting %lu ms).\n", command.servo1Ms);
            break;
        case Step::CloseServo1:
            Serial.printf("Feeding automation step: CloseServo1 (closing servo 1).\n");
            break;
        case Step::StartMotorForward:
            Serial.printf("Feeding automation step: StartMotorForward (starting motor forward).\n");
            break;
        case Step::WaitMotorForward:
            Serial.printf("Feeding automation step: WaitMotorForward (waiting %lu ms).\n", command.stepperMs);
            break;
        case Step::StopMotorForward:
            Serial.printf("Feeding automation step: StopMotorForward (stopping motor).\n");
            break;
        case Step::OpenServo2:
            Serial.printf("Feeding automation step: OpenServo2 (opening servo 2).\n");
            break;
        case Step::WaitServo2:
            Serial.printf("Feeding automation step: WaitServo2 (waiting %lu ms).\n", command.servo2Ms);
            break;
        case Step::CloseServo2:
            Serial.printf("Feeding automation step: CloseServo2 (closing servo 2).\n");
            break;
        case Step::StartMotorReverse:
            Serial.printf("Feeding automation step: StartMotorReverse (starting motor reverse).\n");
            break;
        case Step::WaitMotorReverse:
            Serial.printf("Feeding automation step: WaitMotorReverse (waiting %lu ms).\n", command.stepperMs);
            break;
        case Step::StopMotorReverse:
            Serial.printf("Feeding automation step: StopMotorReverse (stopping motor).\n");
            break;
        }
    }

    void enterStep(Step next)
    {
        step = next;
        logStepEntry(step);
    }

    void waitFor(Step next)
    {
        stepStartedAt = millis();
        enterStep(next);
    }

    bool hasElapsed(unsigned long duration)
    {
        return millis() - stepStartedAt >= duration;
    }

    void startMotor(int state)
    {
        motorStoppedByLimitter = false;
        fed::writeMotor(state);
    }

    void stopMotorByLimitter(const char *side)
    {
        if (motorStoppedByLimitter) return;
        motorStoppedByLimitter = true;
        fed::writeMotor(0);
        Serial.printf("Feeding automation: %s limitter contact detected; motor stopped until duration ends.\n", side);
    }

    void startSequence(const Command &next)
    {
        command = next;
        stepStartedAt = millis();

        Serial.printf(
            "Feeding automation started: servo1=%lu, servo2=%lu, stepper=%lu.\n",
            command.servo1Ms,
            command.servo2Ms,
            command.stepperMs);
        enterStep(Step::OpenServo1);
    }

    void loopSequence()
    {
        switch (step)
        {
        case Step::Idle:
        {
            break;
        }
        case Step::OpenServo1:
        {
            fed::writeServo(servo1OpenInput);
            waitFor(Step::WaitServo1);
            break;
        }
        case Step::WaitServo1:
        {
            if (hasElapsed(command.servo1Ms))
                enterStep(Step::CloseServo1);
            break;
        }
        case Step::CloseServo1:
        {
            fed::writeServo(servo1CloseInput);
            enterStep(Step::StartMotorForward);
            break;
        }
        case Step::StartMotorForward:
        {
            startMotor(1);
            waitFor(Step::WaitMotorForward);
            break;
        }
        case Step::WaitMotorForward:
        {
            if (fed::readRightLimitter())
                stopMotorByLimitter("right");

            if (hasElapsed(command.stepperMs))
                enterStep(Step::StopMotorForward);
            break;
        }
        case Step::StopMotorForward:
        {
            fed::writeMotor(0);
            enterStep(Step::OpenServo2);
            break;
        }
        case Step::OpenServo2:
        {
            fed::writeServo(servo2OpenInput);
            waitFor(Step::WaitServo2);
            break;
        }
        case Step::WaitServo2:
        {
            if (hasElapsed(command.servo2Ms))
                enterStep(Step::CloseServo2);
            break;
        }
        case Step::CloseServo2:
        {
            fed::writeServo(servo2CloseInput);
            enterStep(Step::StartMotorReverse);
            break;
        }
        case Step::StartMotorReverse:
        {
            startMotor(-1);
            waitFor(Step::WaitMotorReverse);
            break;
        }
        case Step::WaitMotorReverse:
        {
            if (fed::readLeftLimitter())
                stopMotorByLimitter("left");

            if (hasElapsed(command.stepperMs))
                enterStep(Step::StopMotorReverse);
            break;
        }
        case Step::StopMotorReverse:
        {
            fed::writeMotor(0);
            enterStep(Step::Idle);
            Serial.printf("Feeding automation finished.\n");
            break;
        }
        }
    }

    bool isSequenceRunning()
    {
        return step != Step::Idle;
    }
}
