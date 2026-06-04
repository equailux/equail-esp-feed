#include "sequence.h"
#include "./limitter.h"
#include "./motor.h"
#include "./refiller.h"
#include "./distributer.h"
#include <Arduino.h>

//

namespace fed::internal
{
    enum class Step
    {
        Idle,
        OpenRefiller,
        WaitRefiller,
        CloseRefiller,
        StartMotorForward,
        WaitMotorForward,
        StopMotorForward,
        OpenDistributer,
        WaitDistributer,
        CloseDistributer,
        StartMotorReverse,
        WaitMotorReverse,
        StopMotorReverse,
    };

    const int refillerServoOpenState = 180;
    const int refillerServoCloseState = 0;
    const int distributerServoOpenState = 179;
    const int distributerServoCloseState = 0;

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
        case Step::OpenRefiller:
            Serial.printf("Feeding automation step: OpenRefiller (opening refiller servo).\n");
            break;
        case Step::WaitRefiller:
            Serial.printf("Feeding automation step: WaitRefiller (waiting %lu ms).\n", command.servo1Ms);
            break;
        case Step::CloseRefiller:
            Serial.printf("Feeding automation step: CloseRefiller (closing refiller servo).\n");
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
        case Step::OpenDistributer:
            Serial.printf("Feeding automation step: OpenDistributer (opening distributer servo).\n");
            break;
        case Step::WaitDistributer:
            Serial.printf("Feeding automation step: WaitDistributer (waiting %lu ms).\n", command.servo2Ms);
            break;
        case Step::CloseDistributer:
            Serial.printf("Feeding automation step: CloseDistributer (closing distributer servo).\n");
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
            "Feeding automation started: refiller=%lu, distributer=%lu, stepper=%lu.\n",
            command.servo1Ms,
            command.servo2Ms,
            command.stepperMs);
        enterStep(Step::OpenRefiller);
    }

    void loopSequence()
    {
        switch (step)
        {
        case Step::Idle:
        {
            break;
        }
        case Step::OpenRefiller:
        {
            fed::writeRefillerServo(refillerServoOpenState);
            waitFor(Step::WaitRefiller);
            break;
        }
        case Step::WaitRefiller:
        {
            if (hasElapsed(command.servo1Ms))
                enterStep(Step::CloseRefiller);
            break;
        }
        case Step::CloseRefiller:
        {
            fed::writeRefillerServo(refillerServoCloseState);
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
            enterStep(Step::OpenDistributer);
            break;
        }
        case Step::OpenDistributer:
        {
            fed::writeDistributerServo(distributerServoOpenState);
            waitFor(Step::WaitDistributer);
            break;
        }
        case Step::WaitDistributer:
        {
            if (hasElapsed(command.servo2Ms))
                enterStep(Step::CloseDistributer);
            break;
        }
        case Step::CloseDistributer:
        {
            fed::writeDistributerServo(distributerServoCloseState);
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
