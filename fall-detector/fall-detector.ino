#include <I2Cdev.h>
#include <MPU6050.h>
#include <LiquidCrystal.h>
#include <Wire.h>

// pins used for LCD
#define LCD_RS 8
#define LCD_EN 3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

#define FALL_TIME 250              // time to detect a fall after a high acceleration in milliseconds
#define FALL_HIGH_THRESHOLD 30000  // high acceleration threshold to check for a fall
#define FALL_LOW_THRESHOLD 1000    // low acceleration threshold to detect a fall

// offsets found by holding the accelerometer still
#define ACCEL_X_OFFSET -16000
#define ACCEL_Y_OFFSET 150 
#define ACCEL_Z_OFFSET 8000

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

MPU6050 accelgyro;

// acceleration values from accelerometer
int16_t ax, ay, az;

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

unsigned long counter = 0;
unsigned long fallTimer = 0;

void setup() {
    Wire.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    lcd.begin(16, 2);
}

void loop() {
    // read raw accel/gyro measurements from device
    //accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
 
    accelgyro.getAcceleration(&ax, &ay, &az);
    ax = ax + ACCEL_X_OFFSET;
    ay = ay + ACCEL_Y_OFFSET;
    az = az + ACCEL_Z_OFFSET;
    long accel = sqrt((long)ax*ax + (long)ay*ay + (long)az*az);

    if (millis() - counter > 100) {
      counter = millis();
      // debugging prints
      #ifdef OUTPUT_READABLE_ACCELGYRO
          Serial.print("a/g:\t");
          Serial.print(ax); Serial.print("\t");
          Serial.print(ay); Serial.print("\t");
          Serial.print(az); Serial.print("\t");
          Serial.println(accel);
      #endif
      if (accel > FALL_HIGH_THRESHOLD) {
        fallTimer = millis();
        // check for low acceleration after breaking high threshold for FALL_TIME
        while (millis() - fallTimer < FALL_TIME) {
          accelgyro.getAcceleration(&ax, &ay, &az);
          accel = sqrt((long)ax*ax + (long)ay*ay + (long)az*az);
          // if low acceleration is passed, a fall is detected
          if (accel < FALL_LOW_THRESHOLD) {
            Serial.println("Fall detected");
            lcd.print("Fall detected");
            delay(2000);
            lcd.clear();
            break;
          }
          fallTimer = millis();
        }
      }
    }

    
}
