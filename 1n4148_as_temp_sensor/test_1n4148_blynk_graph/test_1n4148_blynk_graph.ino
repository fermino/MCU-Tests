	/**
	 * Copyright 2020 Fermín Olaiz. 
	 * https://github.com/fermino
	 * 
	 * A CHEAP temperature sensor using a 1N4148 diode and a 33Kohm resistor
	 * 
	 * So, I saw an example of this in the Arduino Create page and took
	 * the concept and rewrote it from scratch. 
	 * 
	 * Connections as follows: 
	 *   - GND to 1N4148 Cathode (the black/white marking)
	 *   - 1N4148 Anode to 33Kohm resistor AND to A0. 
	 *   - 33Kohm resistor to 3V3. 
	 * 
	 * So, it pretty much works this way, if you raise the temp by a celcius degree, 
	 * the diode forward voltage will drop *around* 2.2mV (I should've measured it, or looked at the actual datasheet but whatever)
	 * Guess I'll do it at some point. 
	 * 
	 * It actually changes based on the forward current but the coefficient is bigger as you lower the forward current
	 * That's why I chose the 33Kohm resistor, as it will make the forward current about 0.1mAh. 
	 *
	 * You calibrate it by giving it a known combination of temperature and analog reading, 
	 * that reading will obviously be related to the power conditions of your circuit,  
	 * so if you change the 3v3 regulator for the esp8266, you should calibrate it again
	 * You can connect it and it will give you the raw readings in the serial port, they are also in the blynk graph. (virtual pin #1)
	 * Also, make sure that you're using the correct reference voltage. Some esp modules have a voltage divider on the analog pin, 
	 * that will give you a reading range from 0V to 3V3 o 5V. 
	 * It will work A LOT better if you connect it straight to the ESP A0 pin (yeah, you can solder it directly to the ESP12-F module pin)
	 * The program will calculate it for both 1.0V and 3.3V. If you use 3.3V, make sure to measure the actual voltage and set it in the defines
	 * 
	 * After having that data, the program will take a couple measures, average them (it's not pretty but is a TEST, hey :c)
	 * It will take the reading, convert it to an ABSOLUTE VOLTAGE DIFFERENCE, based on the previous measurements, 
	 * And then it will divide it by the coefficient factor (2.2mV or whatever you're found is better). 
	 * 
	 * That's all, the explanation is kind of crappy and I probably could've done this a thousand times cleaner but, it's just a test. 
	 * The implementation is super buggy and is probably not the best, as power conditions will for sure change the output. 

	 * To do: 
	 * 	- Is there any way to measure the 1V internal ref of the ESP?, it FOR SURE would work better on an AVR with a calibrated 1.1V internal reference. 
	 * 	- In an AVR, also, could be possible to use the internal gain, to gain (hehe) more resolution. 
	 * 	- I'll try to use this in my https://github.com/fermino/ESP-AutoWater project. 
	 *		It will be only for reporting, nothing critical, so this is a FREAKING CHEAP way to get a temperature sensor. 
	 *		Just to find out that it has an internal temp sensor, at least the ESP32. 
	 *		Anyways...
	 *
	 * Oh, and yes, for Blynk, it updates the data every second to a graph with the following virtual pins. 
	 * 3 parameters, as follows: 
	 *    * V0 = Actual temperature. 
	 * 	  * V1 = The actual reading from analogRead(A0);
	 *    * V2 = The reading difference in mV. 
	 */

	#define TEMP_REF	13.0 // C°

	#define READ_REF_1V		522.0
	#define REF_1V			1.0	// volts

	#define READ_REF_3V3	172.0
	#define REF_3V3			3.32 // volts

	// coeficiente en mV/C°
	#define DIODE_TEMP_COEFF	2.2

	// Blynk auth data
	#define BLYNK_HOST	IPAddress(10, 0, 2, 4)
	#define BLYNK_TOKEN	"fflpSV3fjBtF6Zz-BKC3_F0xdTuD_8ap"
	/// Uncomment to enable debugging. 
	#define BLYNK_PRINT Serial

	#include "SimpleSerialDebug.h"

	#include <ESP8266WiFi.h>

	#include <DNSServer.h>
	#include <ESP8266WebServer.h>
	#include <WiFiManager.h>

	#include <BlynkSimpleEsp8266.h>

	void setup()
	{
		Serial.begin(115200);

		// Setup WiFiManager (Yeap, this lib is awesome!)
		WiFiManager wifi_manager;
		wifi_manager.autoConnect();

		// Setup and connect Blynk
		Blynk.config(BLYNK_TOKEN, BLYNK_HOST);
		Blynk.connect(); // while(!Blynk.connect());?
	}

	void loop()
	{
		Blynk.run();

		uint16_t read1 = analogRead(A0);
		delay(20);
		uint16_t read2 = analogRead(A0);
		delay(20);
		uint16_t read3 = analogRead(A0);
		delay(20);
		uint16_t read4 = analogRead(A0);
		delay(20);
		uint16_t read5 = analogRead(A0);
		uint16_t read = (read1 + read2 + read3 + read4 + read5) / 5;

		float read_diff_1V = ((float) read - READ_REF_1V) * (REF_1V / 1024.0);
		float temp_1V = TEMP_REF - ((read_diff_1V * 1000) / (DIODE_TEMP_COEFF));

		float read_diff_3V3 = ((float) read - READ_REF_3V3) * (REF_3V3 / 1024.0);
		float temp_3V3 = TEMP_REF - ((read_diff_3V3 * 1000) / DIODE_TEMP_COEFF);

		Serial.print("R 1V: ");
		Serial.print(temp_1V);
		Serial.print("\t R 3V3:");
		Serial.print(temp_3V3);
		Serial.print("\t Raw:");
		Serial.print(read);

		Blynk.virtualWrite(V0, temp_1V);
		Blynk.virtualWrite(V1, read);
		Blynk.virtualWrite(V2, ((float) read - READ_REF_1V) * (REF_1V / 1024.0));

		Serial.println("");
		delay(900);
	}