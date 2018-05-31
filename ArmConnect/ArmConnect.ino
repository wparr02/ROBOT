// Default start values ie 'zero' - middle of stepper motors range of motion required
// Limiting values, based off checking if arm has exceeded it's ROM - Checking number of steps remaining to complete ROM against the current position
// Dual Z-axis
// Parser to handle input string of format "0.00-0.00-0.00-0.00-0.00"
// MORE MOTORS?

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

bool motorFlag[5] = {false};

class motor
{
  // Constructor to take input of "motor type" to class
  public: motor(int motorType)
  {
    switch(motorType)
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
 
  public: void moveMotor(double noStep, int isNegative = 1)
  {
  // Determine No# of step regardless of direction
  int x = noStep * 100 * isNegative;
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
  
  for(int i = 0; i < x; i++)
  {
    digitalWrite(mStep, HIGH);
    delay(1);
    digitalWrite(mStep, LOW);
    delay(1);
  }
  //Serial.println("!");
  
  // Cleanup: Disable output and reset flag.
  motorFlag[motorno] = false;
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
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  
  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(X_DIR_PIN, HIGH);
  digitalWrite(Y_ENABLE_PIN, LOW);
  digitalWrite(Y_DIR_PIN, HIGH);
  Serial.println("HELLO");
}

void loop() {
  double data[5] = {};
  int isneg[5] = {1, 1, 1, 1, 1};
  
  if(Serial.available() > 0)
  {
    int i = 0;
    Serial.flush();
    String serial = Serial.readString();
    Serial.println(serial);
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


    if(data[0] != 0)
    {
          motorFlag[0] = !motorFlag[0];
          //Serial.println(data[0]);
          if (data[0] < 0)
          {
            isneg[0] = -1;
          }
    }
    if(data[1] != 0)
    {
          motorFlag[1] = !motorFlag[1];
          //Serial.println(data[1]);
          if (data[1] < 0)
          {
            isneg[1] = -1;
          }
    }
    if(data[2] != 0)
    {
          motorFlag[2] = !motorFlag[2];
          //Serial.println(data[2]);
          if (data[2] < 0)
          {
            isneg[2] = -1;
          }
    }
    if(data[3] != 0)
    {
          motorFlag[3] = !motorFlag[3];
          //Serial.println(data[3]);
          if (data[3] < 0)
          {
            isneg[3] = -1;
          }
    }
    if(data[4] != 0)
    {
          motorFlag[4] = !motorFlag[4];
          //Serial.println(data[4]);
          if (data[4] < 0)
          {
            isneg[4] = -1;
          }
    }
   
  }
  
  if (motorFlag[0] == true)
    A.moveMotor(data[0], isneg[0]);
    B.moveMotor(data[1], isneg[1]);
    //C.moveMotor(data[2], isneg[2]);
    //D.moveMotor(data[3], isneg[3]);
    //E.moveMotor(data[4], isneg[4]);
}
