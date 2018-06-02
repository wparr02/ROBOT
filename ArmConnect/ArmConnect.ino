#include <Servo.h>

// Class is complete, could be moved to a seperate .h file along with pin definitions; and included in the main sketch to tidy up the main codebase.
// Taking commands of type "0.00/0.00/0.00/0.00/0.00" via serial currently function.

// Further work required on the "END" command, which should take the current stored values of each motor's position (relative to position zero) and rotate in the opposite direct; towards origin.
// Additionally, "RESET" command requires implementing - to provide means of reseting the bool variable endFlag to false; thus re-enableing the arm
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

// How simultaneous the motors should actuate. Ie enables motors to appear to operate simulataneously.
const int simultConst = 100;

double xCurrent = 0;
double yCurrent = 0;
double zCurrent = 0;
double e0Current = 0;
double e1Current = 0;

bool motorFlag[5] = {false};
bool endFlag = false;

Servo gripperServo;

// Class to create different instances of each unique motor, enables functions to be compatible accross all pins
class motor
{
    // Constructor to take input of "motor type" to class. Case 0, 1, 2, 3, 4 correlating to the X, Y, Z, E0 and E1 drivers.
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
    // Initalise some variables used in the constructor
  public:
    int mStep;
    int mDir;
    int mEn;
    int motorno;

    // Class method to actuate the motor, in both CW and CCW rotations. Moves a stepper a total of noStep * stepscaler number of steps. Ie as stepscaler defaults to 100
    // noStep = 10, steps the motor a total of 1000 steps
  public: void moveMotor(double noStep, double& mPosition, int isNegative = 1, int stepscaler = 100) // motor Position, passed as reference to enable tracking of delta from origin
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
        // Is it possible to reduce this delay any further? 
        delay(1);
        digitalWrite(mStep, LOW);
      }


      // Cleanup: Disable output and reset flag.
      //motorFlag[motorno] = false; reset flag elsewhere. To enable pseudo-threading.
      digitalWrite(mEn, HIGH);
    }
};

motor A(0); // Motor X
motor B(1); // Motor Y
motor C(2); // Motor Z
motor D(3); // Motor E0
motor E(4); // Motor E1

void setup() {
  Serial.begin(9600);

  //gripper servo
  pinMode(11, OUTPUT);
  gripperServo.attach(11);

  pinMode(LED_PIN, OUTPUT);
  Serial.println("HELLO");
  
  // RAMPS 1.4 GPIO Configuration
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
    // Block reading of the buffer until the message has been recieved
    Serial.flush();
    String serial = Serial.readString();

    // Given case, serial == numberic values and end of routine flag not raised - parse the recieved string and store the contained data an array of doubles
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
    // If the command "END" has been recieved from the Python companion programme, reset each stepper motor to the initial starting position.
    // Ie. move an equal and opposite number of steps as the current position, relative to the origin: 0.
    // Currently assigns var data in range 1000, 2000 etc... Should take 10, 20, etc. So... Divide by 100 == fixed? ¯\_(ツ)_/¯
    if (serial == "END" && endFlag != true)
    {
      serial = "";
      Serial.println("END");
      data[0] = -1 * xCurrent / 100;
      data[1] = -1 * yCurrent / 100;
      data[2] = -1 * zCurrent / 100;
      data[3] = -1 * e0Current / 100;
      data[4] = -1 * e1Current / 100;
      endFlag = true;
      Serial.println(endFlag);
    }

    // Hacky fix. To enable multimotor support. Currently polls each idex of the data array to check for non zero values, to trigger the actuation of the motor.
    if (data[0] != 0)
    {
      motorFlag[0] = !motorFlag[0];
      if (data[0] < 0)
      {
        isneg[0] = -1;
      }
      // Hacky fix. To enable simultaneous rotation, divide the total number of steps accross a pool of "threads".
      // Slower single motor speed, if all motors are running - however all appear to move simultaneous. Time to complete operation would be of a neglible delta 
      // compared against individual motor total time of operation
      data[0] = data[0] / simultConst;
    }
    if (data[1] != 0)
    {
      motorFlag[1] = !motorFlag[1];
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
  // Servo commands
  //gripperServo.write(0);
  //gripperServo.write(180);
  
  for (int i = 0; i < simultConst; i++)
  {
    // Hacky fix. Implemented to check each index of the motor flag array, to determine which motors to acuate.
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
  // Attempted hacky fix. Bug with the "END" command causing the programme to load data array. Perform a loop back to origin, and then continuing to loop until arduino is reset. Infinite loop has been achieved.
  // ¯\_(ツ)_/¯
  motorFlag[0] = false;
  motorFlag[1] = false;
  motorFlag[2] = false;
  motorFlag[3] = false;
  motorFlag[4] = false;
}
