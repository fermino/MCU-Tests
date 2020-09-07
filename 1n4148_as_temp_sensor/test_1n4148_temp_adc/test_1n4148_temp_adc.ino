	//test_1n4148_temp_adc.ino

	#define TEMP_REF	15.0		// Por ejemplo, ponele...
	#define READ_REF_1V		525.0
	#define READ_REF_3V3	172.0
	 // coeficiente en mV/C°
	#define DIODE_TEMP_COEFF	2.2

	void setup() {
	  // put your setup code here, to run once:
	  Serial.begin(115200);
	}

	void loop()
	{
		uint16_t read1 = analogRead(A0);
		uint16_t read2 = analogRead(A0);
		uint16_t read3 = analogRead(A0);
		uint16_t read4 = analogRead(A0);
		uint16_t read5 = analogRead(A0);
		uint16_t read = (read1 + read2 + read3 + read4 + read5) / 5;

		float read_diff_1V = ((float) read - READ_REF_1V) * (1.0 / 1024.0);
		float temp_1V = TEMP_REF - ((read_diff_1V * 1000) / (DIODE_TEMP_COEFF));

		float read_diff_3V3 = ((float) read - READ_REF_3V3) * (3.32 / 1024.0);
		float temp_3V3 = TEMP_REF - ((read_diff_3V3 * 1000) / DIODE_TEMP_COEFF);

		Serial.print("R 1V: ");
		Serial.print(temp_1V);
		Serial.print("\t R 3V3:");
		Serial.print(temp_3V3);
		Serial.print("\t Raw:");
		Serial.print(read);

		/*Serial.print("1V reading: ");
		Serial.print(temp_1V);
		Serial.print("C\t");
		Serial.print("3V3 reading: ");
		Serial.print(temp_3V3);
		Serial.println("C");*/

		Serial.println("");

		delay(500);
	}