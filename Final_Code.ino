  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>

  // Two LCDs with addresses
  LiquidCrystal_I2C lcd1(0x25, 16, 2);  // LCD1 for startup msg
  LiquidCrystal_I2C lcd2(0x27, 16, 2);  // LCD2 for startup msg

  // Pins
  const int moisturePin = A1;
  const int voltagePin = A0;
  const int relayPin = 2;

  // Constants
  const float VOLTAGE_RATIO = 5.0;     
  const float ARDUINO_REF_VOLTAGE = 5.0;  
  const float ADC_RESOLUTION = 1023.0;    
  const float RESISTANCE = 35.0;  // Ohms

  // Pump thresholds
  const int dryThreshold = 750;  
  const int wetThreshold = 750;  

  // Fixed input power
  const float INPUT_VOLTAGE = 9.0;  // Volts
  const float INPUT_CURRENT = 0.25;  // Amps
  const float INPUT_POWER = INPUT_VOLTAGE * INPUT_CURRENT;  // 2.7 Watts

  bool pumpOn = false;

  void setup() {
    Serial.begin(9600);

    Wire.begin();

    // Init LCDs and display fixed messages once
    lcd1.init();
    lcd1.backlight();
    lcd1.setCursor(0, 0);
    lcd1.print("Irrigation");
    lcd1.setCursor(0, 1);
    lcd1.print("System");

    lcd2.init();
    lcd2.backlight();
    lcd2.setCursor(0, 0);
    lcd2.print("Check Serial");
    lcd2.setCursor(0, 1);
    lcd2.print("Monitor");

    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH);  // Pump OFF

    delay(2000);  // Show startup for 2 seconds

    // DO NOT clear or update LCD hereafter to keep messages steady

    Serial.println("System Started");
  }

  void loop() {
    int moistureValue = analogRead(moisturePin);

    // Pump control
    if (moistureValue > dryThreshold && !pumpOn) {
      pumpOn = true;
      digitalWrite(relayPin, LOW);  // Pump ON
      Serial.println("Pump ON (Soil Dry)");
    } else if (moistureValue < wetThreshold && pumpOn) {
      pumpOn = false;
      digitalWrite(relayPin, HIGH);  // Pump OFF
      Serial.println("Pump OFF (Soil Wet)");
    }

    // Voltage reading
    int rawVoltageValue = analogRead(voltagePin);
    float voltage = (rawVoltageValue / ADC_RESOLUTION) * ARDUINO_REF_VOLTAGE * VOLTAGE_RATIO;

    // Current calculation
    float current = voltage / RESISTANCE;

    // Output power
    float outputPower = voltage * current;

    // Efficiency
    float efficiency = 0.0;
    if (INPUT_POWER > 0) {
      efficiency = (outputPower / INPUT_POWER) * 100.0;
    }

    // Serial output
    Serial.print("Moisture: ");
    Serial.print(moistureValue);
    Serial.print(" | V: ");
    Serial.print(voltage, 2);
    Serial.print(" V | I: ");
    Serial.print(current, 3);
    Serial.print(" A | Efficiency: ");
    Serial.print(efficiency, 1);
    Serial.println(" %");

    delay(1000);  // 1 second delay
  }
