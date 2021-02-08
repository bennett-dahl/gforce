// #include <Arduino.h>
#include <M5Stack.h>
#include <gSense.h>
#include <Event.h>
#include <accData.h>

MPU6886 mpu;

Event updateAccelDataEvent; //new global variables of the Event structure for holding the event information
Event xTimedPeak;

accData data;

unsigned long baseClock;       //place to hold millis() each time through the loop
unsigned int bufferTime = 500; //time in MS for reading to update data.timePeakX/Y/Z

//function prototypes
void checkEventTimers();
void manageFlags();
void manageEvents();
void updateAccelData();
void updateTimedPeakVal();
void updateDisplay();

void setup()
{
  Serial.begin(115200);
  M5.begin();
  Wire.begin();

  M5.Lcd.begin();
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(3);

  mpu.Init();

  updateDisplay();

  updateAccelDataEvent.frequency = 50; //this sets the frequency of the events
  xTimedPeak.frequency = bufferTime;
}

//check out the "pass by reference" portion of my email for a closer explination of the & here.
//this function takes an "Event" type variable, and asses if enough time has passed since the last trigger and sets the flag if so
//you'll notice this is the same as the code that was duplicated in the last example.
void checkEventTimers(Event &event)
{
  baseClock = millis();
  unsigned int timeDelta = baseClock - event.timer;
  if (timeDelta > event.frequency)
  {
    event.flag = true;
    event.timer = millis();
  }
}

//this function just monitors timings, and sets the flags for events when they should happen
void manageFlags()
{
  checkEventTimers(updateAccelDataEvent); //instead of having all that duplicate code, we just pass the events to the function we made.
  if (xTimedPeak.runTimer)
    checkEventTimers(xTimedPeak); //only time this out if runTimer is true
}
//this function executes functions based upon if the flags are set or not.
void manageEvents()
{
  if (updateAccelDataEvent.flag)
  {
    updateAccelData();
  }
  if (xTimedPeak.flag)
  {
    updateTimedPeakVal();
  }
}

void updateAccelData()
{
  mpu.getAccelData(&data.aX, &data.aY, &data.aZ);
  // Serial.printf("%f, %f, %f\n", data.peakX, data.peakY, data.peakZ);
  if (fabs(data.aX) > data.peakX)
  {
    data.peakX = fabs(data.aX);
    updateDisplay();
  }
  if (fabs(data.aY) > data.peakY)
  {
    data.peakY = fabs(data.aY);
    updateDisplay();
  }
  if (fabs(data.aZ) > data.peakZ)
  {
    data.peakZ = fabs(data.aZ);
    updateDisplay();
  }
  if (fabs(data.aX) > data.timePeakX)
  {
    xTimedPeak.runTimer = true;
    Serial.println("Timing...");
  }
  else
  {
    xTimedPeak.runTimer = false;
    xTimedPeak.timer = millis(); //constantly keep timer update with current time.
  }
  updateAccelDataEvent.flag = false; //make sure to clear the flag after you execute the event function
}

void updateTimedPeakVal()
{

  data.timePeakX = fabs(data.aX);
  // data.timePeakY = data.aY;
  // data.timePeakZ = data.aZ;
  // Serial.printf("New Time Peak %f, %f, %f\n", data.timePeakX, data.timePeakY, data.timePeakZ);
  xTimedPeak.flag = false; //make sure to clear the flag after you execute the event function
}

void updateDisplay()
{
  M5.Lcd.clear();
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println(" G-Force Testing");
  M5.Lcd.setCursor(60, 60);
  M5.Lcd.printf("X: %f", data.peakX);
  M5.Lcd.setCursor(60, 100);
  M5.Lcd.printf("Y: %f", data.peakY);
  M5.Lcd.setCursor(60, 140);
  M5.Lcd.printf("Z: %f", data.peakZ);
}

void loop()
{
  manageFlags();  //handles timing every time through the loop
  manageEvents(); //looks for enabled flags, and executes events based on that

  // put your main code here, to run repeatedly:
}