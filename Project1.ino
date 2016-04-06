
/*
 * @name Baby Monitor App
 * @author Libor Kampas
 * @date 06 April 2016
 * @version 1.0
 * Sensors used: Temperature (A0), potentiometer (A3), sound (A2), button (D2), touch (D8)
 * Outputs used: Red (D6) and Green (D5) LED, buzzer (D3), LCD (I2C)
 */

//  "Threads", requires a Timer Interrupt Library. 
#include <TimerOne.h>

//LCD display required libraries
#include <Wire.h>
#include "rgb_lcd.h"

#include "Thread.h"
#include "ThreadController.h"
/*
	This is a more "complex" for of using Threads.
	You can also inherit from Thread, and do your entire code on the class.

	This allows you, to create for example:
		Sensor Readings (aquire, filter, and save localy values)
		Custom Blinks, Beeps...
		Anything you can imagine.

	Threads are more "usefull" when used within Timer interrupts

	This way of coding is more "reusable", and "correct" (Object Oriented)
*/

//Temperature sensor pin declaration
const int tempSensorPin = A0;
//initialize the temperature bounderies
float lowComfort = 19;
float highComfort = 21.9;
//temperature constant
const int B = 3975;

//instantiate lcd
rgb_lcd lcd;
const int colorR = 0;
const int colorG = 200;       //set the display background to green
const int colorB = 0;

//LED pins declaration
const int pinLedGreen = 5;
const int pinLedRed = 6;
// Define the delay for the "breathing" effect; change this
// to a smaller value for a faster effect, larger for slower.
const int BREATH_DELAY = 1;    // milliseconds

//LCD pin declaration
const int buttonPin = 2;      // the number of the pushbutton pin
int lcdState = 1;             //lcd is ON by default

//Touch sensor pin declaration
const int touchPin = 8;       // the number of the touch sensor pin
//Buzzer pin declaration
const int buzzerPin = 3;  // the number of the buzzer pin
int buzzerState = 0;          //buzzer is on default OFF

//Sound sensor pin declaration
const int soundSensorPin = A2;
int soundLevel = 0;         //initialize the sound level to quite

//Potentiometer pin declaration
const int potentiometerPin = A3;
int potentiometerLevel = 0;   //Initialize the potentiometer level

// Define the sound level above which to turn on the Buzzer
// Change this to a larger value to require a louder noise level.
int thresholdValue = 170;
  
// Create a new Class, called AnalogSensorThread, that inherits from Thread
class AnalogSensorThread: public Thread
{
public:
	int value;
	int pin;

	void run(){
		// Reads the analog pin, and saves it localy
		value = analogRead(pin);
		runned();
	}
};

// Create a new Class, called DigitalSensorThread, that inherits from Thread
class DigitalSensorThread: public Thread
{
public:
  int value;
  int pin;

  void run(){
    // Reads the digital pin, and saves it localy
    value = digitalRead(pin);
    runned();
  }
};

// Now, let's use our new class of Thread
AnalogSensorThread analog1 = AnalogSensorThread();
AnalogSensorThread analog2 = AnalogSensorThread();
AnalogSensorThread analog3 = AnalogSensorThread();
DigitalSensorThread digital1 = DigitalSensorThread();
DigitalSensorThread digital2 = DigitalSensorThread();

// Instantiate a new ThreadController
ThreadController controller = ThreadController();

// This is the callback for the Timer
void timerCallback(){
	controller.run();
}

void setup(){

  //Set a serial data rate
	Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB); //default is green screen
  lcd.print("  Baby Monitor");

  // Configure the LED's pin for output signals.
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedRed, OUTPUT);

  // Configure the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // Configure the touchbutton pin as an input:
  pinMode(touchPin, INPUT);
  // Configure the potentiometer pin as an input:
  pinMode(potentiometerPin, INPUT);

  //Configure the buzzer pin as an output
  pinMode(buzzerPin, OUTPUT);
    
	// Configures Thread analog1
	analog1.pin = tempSensorPin;
	analog1.setInterval(100);

	// Configures Thread analog2
	analog2.pin = soundSensorPin;
	analog2.setInterval(100);

  // Configures Thread analog3
  analog3.pin = potentiometerPin;
  analog3.setInterval(100);

  //Configures Thread digital1
  digital1.pin = buttonPin;
  digital1.setInterval(100);

  //Configures Thread digital2
  digital2.pin = touchPin;
  digital2.setInterval(100);

	// Add the Threads to our ThreadController
	controller.add(&analog1);
	controller.add(&analog2);
  controller.add(&analog3);
  controller.add(&digital1);
  controller.add(&digital2);

	 Timer1.initialize(20000);
	 Timer1.attachInterrupt(timerCallback);
	 Timer1.start();

}

void loop(){
	// Do complex-crazy-timeconsuming-tasks here
	delay(1000);
	
	// Get the fresh readings
  int tempValue = analog1.value;
  // Determine the current resistance of the thermistor based on the sensor value.
  float resistance = (float)(1023-tempValue)*10000/tempValue;
  // Calculate the temperature based on the resistance value.
  float temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;
  Serial.print("Analog1 Thread Temperature: ");
	Serial.print(temperature);
  Serial.println(" degrees.");

	Serial.print("Analog2 Thread Sound level: ");
	Serial.println(analog2.value);
  soundLevel = analog2.value;

  Serial.print("Analog3 Thread Potentiometr level: ");
  /**
   * total range is 0-1023, we need to find a half( roughly 511)
   * division by 100 will result in range +5/-5
   * since potentiometer value increases by turning to the left, 
   * we divide by negative number to reverse that and we control the potentiometer intuitavly
   */
  Serial.println((analog3.value-511) / -100); //creates number between -5 and +5
  potentiometerLevel = (analog3.value-511) / -100;
  
  Serial.print("Digital1 Thread Button: ");
  int buttonState = digital1.value;
  Serial.println(buttonState);

  Serial.print("Digital2 Thread Touch: ");
  int touchState = digital2.value;
  Serial.println(touchState);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Low Temp: ");
  lcd.print(lowComfort + potentiometerLevel);
  lcd.println("°");
  lcd.setCursor(0, 1);
  lcd.print("High Temp:");
  lcd.print(highComfort + potentiometerLevel);
  lcd.println("°");
  delay(3000);

  //if button is pressed, the button state is HIGH
  if (touchState == 1) 
  {
    if (buzzerState == 0)
    {
      buzzerState = 1;  //buzzer can be switched on
    }
    else
    {
      buzzerState = 0;  //buzzer cannot be switched on
    }
  }

  //if noice level is high
  if (soundLevel >= thresholdValue && buzzerState == 1)
  {
    Serial.println("Buzzer on.");
    digitalWrite(buzzerPin, HIGH);
    delay(analogRead(0));
    digitalWrite(buzzerPin, LOW);
    delay(analogRead(0));
  }
  else
  {
    Serial.println("Buzzer off.");
  }

  //if button is pressed, the button state is HIGH
  if (buttonState == HIGH) 
  {
    if (lcdState == 0)
    {
      lcd.display();
      lcdState = 1;
    }
    else
    {
      lcd.noDisplay();
      lcd.setRGB(0, 0, 0);  //using hard coded numbers to simplify the code
      lcdState = 0;
    }
      
  }

  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room Temperature:");
  if (temperature < (lowComfort + potentiometerLevel) || temperature > (highComfort + potentiometerLevel))
  {
      //Set display to red background if too hot and blue if too cold. Blue color added after presentation.
      if (temperature < (lowComfort + potentiometerLevel) && lcdState == 1)  //set background only if display is on
        {
          lcd.setRGB(0, 0, 200);  //using hard coded numbers to simplify the code
        }
        else if (temperature > (highComfort + potentiometerLevel) && lcdState == 1)  //set background only if display is on
        {
          lcd.setRGB(200, 0, 0);  //using hard coded numbers to simplify the code
        }
        
      analogWrite(pinLedGreen, 0);
      
      for(int i=0; i<256; i++)
      {
        analogWrite(pinLedRed, i);
        delay(BREATH_DELAY);
      }
      delay(100);
    
      for(int i=254; i>=0; i--)
      {
        analogWrite(pinLedRed, i);
        delay(BREATH_DELAY);
      }
      delay(500);
  }
  else
  {
      //Set display to green background
      if (lcdState == 1)  //set background only if display is on
      {
        lcd.setRGB(colorR, colorG, colorB);
      }
      analogWrite(pinLedGreen, 255);
  }
  lcd.setCursor(0, 1);
  lcd.print(temperature);
  
	// Do more complex-crazy-timeconsuming-tasks here
	delay(3000);

}

