//flow_meter_test.ino
	#define FLOW_PIN D8

	volatile uint32_t flow_pulse_count = 0;

	void IRAM_ATTR pulse_receive()
	{ ++flow_pulse_count; }

	void setup()
	{
		Serial.begin(115200);

		pinMode(FLOW_PIN, INPUT_PULLUP);

		attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pulse_receive, FALLING);
	}

	void loop()
	{
		Serial.print("Pulses: ");
		Serial.print(flow_pulse_count);
		Serial.print("\t ml: ");
		Serial.print((float) flow_pulse_count * 2.25);

		Serial.println("");

		delay(1000);
	}