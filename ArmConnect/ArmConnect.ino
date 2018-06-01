#include <Servo.h>

// Default start values ie 'zero' - middle of stepper motors range of motion required
// Limiting values, based off checking if arm has exceeded it's ROM - Checking number of steps remaining to complete ROM against the current position
// Dual Z-axis
// Parser to handle input string of format "0.00-0.00-0.00-0.00-0.00"
// MORE MOTORS?



//
#define LED_PIN            13

#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

#define E0_STEP_PIN        26
#define E0_DIR_PIN         28
#define E0_ENABLE_PIN      24

#define E1_STEP_PIN        36
#define E1_DIR_PIN         34
#define E1_ENABLE_PIN      30

const int simultConst = 100;
double xCurrent = 0;
double yCurrent = 0;
double zCurrent = 0;
double e0Current = 0;
double e1Current = 0;

bool motorFlag[5] = {false};
bool endFlag = false;
Servo gripperServo;

class motor
{
    // Constructor to take input of "motor type" to class
  public: motor(int motorType)
    {
      switch (motorType)
      {
        case 0: mStep = X_STEP_PIN;
          mDir = X_DIR_PIN;
          mEn = X_ENABLE_PIN;
          motorno = 0;
          break;

        case 1: mStep = Y_STEP_PIN;
          mDir = Y_DIR_PIN;
          mEn = Y_ENABLE_PIN;
          motorno = 1;
          break;

        case 2: mStep = Z_STEP_PIN;
          mDir = Z_DIR_PIN;
          mEn = Z_ENABLE_PIN;
          motorno = 2;
          break;

        case 3: mStep = E0_STEP_PIN;
          mDir = E0_DIR_PIN;
          mEn = E0_ENABLE_PIN;
          motorno = 3;
          break;

        case 4: mStep = E1_STEP_PIN;
          mDir = E1_DIR_PIN;
          mEn = E1_ENABLE_PIN;
          motorno = 4;
          break;
      }
    }
  public:
    int mStep;
    int mDir;
    int mEn;
    int motorno;

  public: void moveMotor(double noStep, double& mPosition, int isNegative = 1, int stepscaler = 100)
    {
      // Determine No# of step regardless of direction
      int x = (noStep * stepscaler * isNegative);
      mPosition += x * isNegative;
      //Serial.println(x);
      //Serial.println(isNegative);
      // Determine which direction of rotation. +VE inputs equate to clockwise rotation, -VE inputs equate to counter-clockwise.
      if (isNegative != 1)
      {
        digitalWrite(mDir, LOW);
      }
      else
      {
        digitalWrite(mDir, HIGH);
      }

      // Enable output, and perform x steps
      digitalWrite(mEn, LOW);

      for (int i = 0; i < x; i++)
      {
        digitalWrite(mStep, HIGH);
        delay(1);
        digitalWrite(mStep, LOW);
      }
      //Serial.println("!");

      // Cleanup: Disable output and reset flag.
      //motorFlag[motorno] = false;
      digitalWrite(mEn, HIGH);
    }
};

motor A(0);
motor B(1);
motor C(2);
motor D(3);
motor E(4);

void setup() {
  Serial.begin(9600);

  //gripper servo
  pinMode(11, OUTPUT);
  
  gripperServo.attach(11);
  
  pinMode(LED_PIN, OUTPUT);
  Serial.println("HELLO");

  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);

  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);

  pinMode(Z_STEP_PIN, OUTPUT);
  pinMode(Z_DIR_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);

  pinMode(E0_STEP_PIN, OUTPUT);
  pinMode(E0_DIR_PIN, OUTPUT);
  pinMode(E0_ENABLE_PIN, OUTPUT);

  pinMode(E1_STEP_PIN, OUTPUT);
  pinMode(E1_DIR_PIN, OUTPUT);
  pinMode(E1_ENABLE_PIN, OUTPUT);

  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(X_DIR_PIN, HIGH);

  digitalWrite(Y_ENABLE_PIN, LOW);
  digitalWrite(Y_DIR_PIN, HIGH);

  digitalWrite(Z_ENABLE_PIN, LOW);
  digitalWrite(Z_DIR_PIN, HIGH);

  digitalWrite(E0_ENABLE_PIN, LOW);
  digitalWrite(E0_DIR_PIN, HIGH);

  digitalWrite(E1_ENABLE_PIN, LOW);
  digitalWrite(E1_DIR_PIN, HIGH);

}

void loop() {
  double data[5] = {};
  int isneg[5] = {1, 1, 1, 1, 1};

  if (Serial.available() > 0)
  {
    int i = 0;
    Serial.flush();
    String serial = Serial.readString();
    //Serial.println(serial);
    if (serial != "END" && endFlag == false)
    {
      char str[serial.length()];
      serial.toCharArray(str, serial.length());
      char* token = strtok(str, "/");
      char* tokArray [5];

      while (token != NULL)
      {
        tokArray[i++] = token;
        token = strtok(NULL, "/");
      }

      data[0] = atof(tokArray[0]);
      data[1] = atof(tokArray[1]);
      data[2] = atof(tokArray[2]);
      data[3] = atof(tokArray[3]);
      data[4] = atof(tokArray[4]);
    }
    if (serial == "END" && endFlag != true)
    {
      serial = "";
      Serial.println("END");
      data[0] = -1 * xCurrent;
      data[1] = -1 * yCurrent;
      data[2] = -1 * zCurrent;
      data[3] = -1 * e0Current;
      data[4] = -1 * e1Current;
      endFlag = true;
      Serial.println(endFlag);
    }

    if (data[0] != 0)
    {
      motorFlag[0] = !motorFlag[0];
      if (data[0] < 0)
      {
        isneg[0] = -1;
      }
      data[0] = data[0] / simultConst;
      Serial.println(data[0]);
    }
    if (data[1] != 0)
    {
      motorFlag[1] = !motorFlag[1];
      //Serial.println(data[1]);
      if (data[1] < 0)
      {
        isneg[1] = -1;
      }
      data[1] = data[1] / simultConst;

    }
    if (data[2] != 0)
    {
      motorFlag[2] = !motorFlag[2];
      //Serial.println(data[2]);
      if (data[2] < 0)
      {
        isneg[2] = -1;
      }
      data[2] = data[2] / simultConst;
    }
    if (data[3] != 0)
    {
      motorFlag[3] = !motorFlag[3];
      //Serial.println(data[3]);
      if (data[3] < 0)
      {
        isneg[3] = -1;
      }
      data[3] = data[3] / simultConst;
    }
    if (data[4] != 0)
    {
      motorFlag[4] = !motorFlag[4];
      //Serial.println(data[4]);
      if (data[4] < 0)
      {
        isneg[4] = -1;
      }
      data[4] = data[4] / simultConst;
    }
  }
  gripperServo.write(0);
  for (int i = 0; i < simultConst; i++)
  {
    if (motorFlag[0] == true)
      A.moveMotor(data[0], xCurrent, isneg[0]);
    if (motorFlag[1] == true)
      B.moveMotor(data[1], yCurrent, isneg[1]);
    if (motorFlag[2] == true)
      C.moveMotor(data[2], zCurrent, isneg[2]);
    if (motorFlag[3] == true)
      D.moveMotor(data[3], e0Current, isneg[3]);
    if (motorFlag[4] == true)
      E.moveMotor(data[4], e1Current, isneg[4]);
  }
  gripperServo.write(180);
 
  //Serial.println(xCurrent);
  motorFlag[0] = false;
  motorFlag[1] = false;
  motorFlag[2] = false;
  motorFlag[3] = false;
  motorFlag[4] = false;
}
