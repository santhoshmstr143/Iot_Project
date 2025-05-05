#define MIC_AO 12               // Microphone Analog Output connected to GPIO12
#define GUNSHOT_THRESHOLD 2035 // Threshold for gunshot detection
const int analogPin = 32;      // Gas sensor analog input

#define LED1 5     // LED connected to GPIO5
#define LED2 16    // LED connected to GPIO16
#define BUZZER 4   // Buzzer connected to GPIO4

void setup() {
    Serial.begin(115200);
    Serial.println("Gunshot & Air Quality Monitoring Started...");

    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    digitalWrite(LED1, LOW);    // Start with LEDs and buzzer off
    digitalWrite(LED2, LOW);
    digitalWrite(BUZZER, LOW);
}

void loop() {
    // === Gunshot Detection ===
    int rawValue = analogRead(MIC_AO);              // Read microphone signal
    int soundLevel = abs(rawValue - 2048);          // Normalize around 2048

    Serial.print(" | Sound Level: ");
    Serial.println(soundLevel);

    if (soundLevel > GUNSHOT_THRESHOLD) {
        Serial.println("🚨 GUNSHOT DETECTED! 🚨");
        digitalWrite(LED1, HIGH);    // Turn ON LEDs
        digitalWrite(LED2, HIGH);
    } else {
        digitalWrite(LED1, LOW);     // Turn OFF LEDs
        digitalWrite(LED2, LOW);
    }

    // === Air Quality Monitoring ===
    int gasValue = analogRead(analogPin);  // Read gas sensor value (0–4095)
    String status;

    if (gasValue < 2000) {
        status = "Clean Air 🟢";
        digitalWrite(BUZZER, LOW);  // No alert
    } else if (gasValue < 2800) {
        status = "Moderate Air 🟡";
        digitalWrite(BUZZER, LOW);  // Still safe
    } else {
        status = "Polluted Air 🔴";
        Serial.println("⚠  ALERT: Poor Air Quality! ⚠");
        digitalWrite(BUZZER, HIGH);  // Trigger buzzer
        delay(200);                  // Short beep
        digitalWrite(BUZZER, LOW);   // Buzzer OFF
    }

    Serial.print("Analog Gas Level: ");
    Serial.print(gasValue);
    Serial.print(" | Air Quality: ");
    Serial.println(status);

    delay(100);  // Short delay for next reading
}