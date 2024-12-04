#include <Arduino.h>

#define NTC_PIN A0
#define HEIZUNG_PIN 2

const float A = 0.00438;
const float B = -0.00026;
const float C = 0.000001756;
const uint16_t resistanceR1 = 12000;

enum states
{
  abkuehlen,
  heizen
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Temperaturmessung");
  pinMode(HEIZUNG_PIN, OUTPUT);
  digitalWrite(HEIZUNG_PIN, LOW);
}

void loop()
{
  static uint8_t state = states::abkuehlen;
  static float temperatureC = 0;
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 500)
  {
    uint16_t voltageADC = analogRead(NTC_PIN);

    float voltageR1 = (5.0 / 1024.0) * (float)voltageADC;

    float voltageNTC = 5.0 - voltageR1; // Achtung bei Low Side Beschaltung sind voltageR1 und voltageNTC vertauscht
    float current = voltageR1 / (float)resistanceR1;

    float resistanceNTC = voltageNTC / current;

    float temperatureK = 1.0 / (A + B * (log(resistanceNTC)) + C * (pow(log(resistanceNTC), 3))); //A, B, C = Koeffizienten der Steinhart Gleichung

    temperatureC = temperatureK - 273.15;

    Serial.print(">Temperature:");
    Serial.println(temperatureC);

    if (temperatureC >= 27.0)
    {
      state = states::abkuehlen;
    }

    if (temperatureC <= 25.0)
    {
      state = states::heizen;
    }
    
    Serial.print(">Heizung:");
    Serial.println(state);

    lastMillis = millis();
  }

  switch (state)
  {
  case states::abkuehlen:
    digitalWrite(HEIZUNG_PIN, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    break;

  case states::heizen:
    digitalWrite(HEIZUNG_PIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    break;
  
  default:
    break;
  }
}