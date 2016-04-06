# IoT_project
Baby Monitor

/*
 * @name Baby Monitor App
 * @author Libor Kampas
 * @date 06 April 2016
 * @version 1.0
 * Sensors used: Temperature (A0), potentiometer (A3), sound (A2), button (D2), touch (D8)
 * Outputs used: Red (D6) and Green (D5) LED, buzzer (D3), LCD (I2C)
 * 
 */

This app measures temperature in a room and outputs to the LCD display. If room too cold, blue background is set on LCD. Red is set if too hot and green if the temperature is within set boundaries. 

If the room temperature is within the bounderies, the green LED is also switched ON and if its outside of the bounderies then flashing red will turn ON instead.

Potentiometer can be used to move the boundaries +-5 degrees celsius.

If baby cries, sound sensor picks up the noice and turns ON the buzzer (scenario is that the buzzer may be located in the living room and the sound sensor in the baby room).

The buzzer is turned OFF as default (general noice in the room while the app is not needed could annoy other family members by setting false buzzing). It can be switched ON and OFF by the touch sensor.

In case the light from the LCD is not wanted, it can be switched OFF and ON by the button sensor.

About Arduino Thread library used:    https://github.com/ivanseidel/ArduinoThread

TODO: Connect the application with Android app
