// IoT Challenge 1; Date: 18/03/2025; Author: Hong Chen
// Version: IoT 01 After Optimised //
#include <esp_now.h>
#include <WiFi.h>
#include <esp_sleep.h>

// Hardware Configuration
#define TRIG_PIN 5
#define ECHO_PIN 18
#define RED_LED_PIN 2
#define GREEN_LED_PIN 4

#define X   (36 % 50 + 5) // Course-specific duty-cycle parameter: 41 seconds
#define DEEP_SLEEP_TIME  1000*X // unit: ms
#define OCCUPANCY_THRESHOLD 50 // unit: cm

#define P_BOOT                  313   // mW 
#define P_SENSOR                394   // mW 
#define P_EMIT                  1220  // mW 
#define P_WIFI_ON               777   // mW 
#define P_WIFI_OFF              308   // mW 
#define P_SLEEP                 60    // mW 

#define BATTERY_ENERGY    (4836 % 5000 + 15000)  // Joule

float P_Active = 0;

// Energy mJ = mW * s
float E_boot = 0;    
float E_sensor = 0;   
float E_emit = 0;        
float E_WIFI_ON = 0;  
float E_WIFI_OFF = 0;  
float E_sleep = 0;    
float E_per_cycle = 0;
float E_active = 0;


float average_power = 0; // mW

float cycles = 0;
float lifetime_seconds = 0; // seconds
float lifetime_hours = 0; // hours
float period = 0; // ms

// Ultrasonic Sensor Variables
unsigned long duration = 0;
float distance = 0;

volatile bool TX_DONE = false; // Transmission status
volatile esp_now_send_status_t TX_STATUS = ESP_NOW_SEND_FAIL; // Transmission result

unsigned long BOOT_Time = millis();
unsigned long BOOT_Time_Length = 0;

unsigned long WIFI_OFF_Time = micros();
unsigned long WIFI_OFF_Time_Length = 0;

unsigned long Active_Cycle_Time = millis();
unsigned long Active_Time = 0;

unsigned long WIFI_ON_Time = millis();
unsigned long WIFI_ON_Time_Length = 0;

unsigned long Transmission_Time = micros();
unsigned long Transmission_Time_Length = 0;

unsigned long Sensor_Time = micros();
unsigned long Sensor_Time_Length = 0;

// Broadcast MAC Address
uint8_t broadcastMac[] = {0x8C, 0xAA, 0xB5, 0x84, 0xFB, 0x90};

// Receive Callback
void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  Serial.print("[RX] From MAC: ");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print(macStr);
  
  Serial.print(" | Receiver Parking Spot Data: ");
  for(int i = 0; i < len; i++) {
    Serial.printf("%02X ", data[i]);
  }

  bool occupied = (data[0] != 0x00); // If first byte is not 0, the spot is occupied
  union {
    uint8_t bytes[4];
    float value;
  } distanceConverter;
  
  memcpy(distanceConverter.bytes, data + 1, 4);
  float receivedDistance = distanceConverter.value;

  // Control LEDs based on occupancy
  digitalWrite(RED_LED_PIN, occupied ? HIGH : LOW);
  digitalWrite(GREEN_LED_PIN, occupied ? LOW : HIGH);

  Serial.println();
  // Print received data
  Serial.printf("[RX] Status: %s | Distance: %.1f cm\r\n", occupied ? "Occupied" : "Free", receivedDistance);
}

// Transmission Callback function
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  TX_DONE = true;
  TX_STATUS = status;
  Transmission_Time_Length = micros() - Transmission_Time;
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("! Transmite Success !");
  } else {
    Serial.println("!!! Transmite Failed !!!");
  }
  Serial.printf("Transmission Time: %lu microseconds\r\n", Transmission_Time_Length);
}

void setup() {
  Active_Cycle_Time = millis(); // Start time for active cycle

  BOOT_Time = millis();   

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  
  Serial.begin(115200);

  Serial.println("\n----------------- Optimised Version START -----------------");

  BOOT_Time_Length = millis() - BOOT_Time;
  Serial.print("BOOT Time: ");
  Serial.println(String(BOOT_Time_Length) + " ms");

  Serial.println("System Ready");
  // // // // // //// // // // // //// // // // // //// // // // // //
  // // // // // //// // // // // //// // // // // //// // // // // //
  
  // measure distance
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  Sensor_Time = micros();
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo signal duration
  duration = pulseIn(ECHO_PIN, HIGH);
  Serial.printf("The time of sensor measurement duration is : %d microseconds\r\n", duration);

  if (duration == 0) {
    Serial.println("PulseIn returned 0, retrying...");
  } else if (duration > 24000) {
    Serial.println("Invalid duration, greater than 24000, retrying...");
  } else {
    Serial.println("Valid duration, processing..."); 
  }
  Sensor_Time_Length = micros() - Sensor_Time;
  Serial.print("Sensor Time : ");
  Serial.println(String(Sensor_Time_Length) + " microseconds"); 

  WIFI_ON_Time = millis();
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed!");
    return;
  }
  // Configure Peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastMac, 6);
  peerInfo.channel = 0;
    
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Add Peer Failed!");
    return;
    }    

  // Register Callback
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  // Calculate distance (Speed of sound = 343 m/s = 0.0343 cm/µs)
  distance = (duration * 0.0343) / 2;
  
  // Prepare data for transmission
  uint8_t data[5];
  data[0] = (distance < OCCUPANCY_THRESHOLD) ? 0x01 : 0x00; // Set occupancy status based on distance
  memcpy(data + 1, &distance, sizeof(distance));

  TX_DONE = false;
  Transmission_Time = micros();
  esp_err_t result = esp_now_send(broadcastMac, data, sizeof(data));
  
  // Wait for transmission result
  while (!TX_DONE && (micros() - Transmission_Time < 100000)) {
    delayMicroseconds(50);
    }
  
  if (result != ESP_OK) {
      Serial.println("[TX] Status: Error: Send request not added to queue!!!");
      return;
    }else{
      Serial.println("[TX] Status: Success: Data successfully added to send queue!");
    }

  // Check transmission result
  if (TX_DONE) {
    if (TX_STATUS == ESP_NOW_SEND_SUCCESS) {
        Serial.println("[TX] Success: Data successfully delivered");
      } else {
        Serial.println("[TX] Failure: No response from the receiver");
      }
    } else {
    Serial.println("[TX] Timeout: No callback received");
    }    
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);    // turn off WiFi
  WIFI_ON_Time_Length = millis() - WIFI_ON_Time;
  Serial.println("Wi-Fi is turned off");  
  Serial.print("WIFI ON Time: ");
  Serial.println(String(WIFI_ON_Time_Length) + " ms");

  WIFI_OFF_Time = micros();
  Active_Time = millis() - Active_Cycle_Time; // active cycle time (ms)
  Serial.println("Active Time: " + String(Active_Time) + " ms"); 

  period = Active_Time + DEEP_SLEEP_TIME; // ms
  Serial.printf("The period is: %.1f ms! \r\n", period); // ms

  Serial.println("Entering Deep Sleep...");
  
  Serial.println("Sleep Time: " + String(DEEP_SLEEP_TIME) + " ms");

  WIFI_OFF_Time_Length = micros() - WIFI_OFF_Time;
  Serial.print("WIFI OFF Time: ");
  Serial.println(String(WIFI_OFF_Time_Length) + " microseconds");

  // Calculate energy consumption in mJ
  E_boot      = P_BOOT      * (BOOT_Time_Length / 1000.0);        // ms → s
  E_sensor    = P_SENSOR    * (Sensor_Time_Length / 1000000.0);   // μs → s
  E_emit = P_EMIT * (Transmission_Time_Length / 1000000.0); // μs → s
  E_WIFI_ON   = P_WIFI_ON   * (WIFI_ON_Time_Length / 1000.0);     // ms → s
  E_WIFI_OFF  = P_WIFI_OFF  * (WIFI_OFF_Time_Length / 1000000.0); // μs → s
  E_active    = E_boot + E_sensor + E_emit + E_WIFI_ON + E_WIFI_OFF;

  P_Active = 1000 * E_active / Active_Time; // unit: mJ/ms

  E_sleep     = P_SLEEP     * (DEEP_SLEEP_TIME / 1000.0);              // ms → s

  E_per_cycle = E_boot + E_sensor + E_emit + E_WIFI_ON + E_WIFI_OFF + E_sleep;
  average_power = 1000 * E_per_cycle/period; // unit:1000 * mJ/ms = mW

  // Displaying energy consumption
  Serial.println("\n------------ Energy Consumption ------------");
  Serial.printf("BOOT:                     %.2f mJ\r\n", E_boot);
  Serial.printf("SENSOR:                   %.2f mJ\r\n", E_sensor);
  Serial.printf("TRANSIMISSION:            %.2f mJ\r\n", E_emit);
  Serial.printf("WIFI ON:                  %.2f mJ\r\n", E_WIFI_ON);
  Serial.printf("WIFI OFF:                 %.2f mJ\r\n", E_WIFI_OFF);
  Serial.printf("SLEEP:                    %.2f mJ\r\n", E_sleep);
  Serial.printf("Active:                   %.2f mJ\r\n", E_active);
  Serial.printf("TOTAL Energy per Cycle:   %.2f mJ\r\n", E_per_cycle);  // unit: mJ
  Serial.printf("The average power for active time is :     %.2f mW !\r\n", P_Active); // mW
  Serial.printf("The average power for period is :          %.2f mW !\r\n", average_power); // mW
  Serial.println("---------------------------------------------\r\n");

  // Check if energy per cycle is zero or negative
  if (E_per_cycle <= 0) {
    Serial.println("Error: Energy per cycle is zero or negative.");
  } else {
    cycles = 1000 * BATTERY_ENERGY / E_per_cycle; // Calculate the number of cycles the battery can last!
    lifetime_seconds = cycles * period/1000.0;  
    lifetime_hours = lifetime_seconds / 3600.0;
  
    // Display battery lifetime estimation
    Serial.println("\n---------- Battery Lifetime Estimation ---------");
    Serial.printf("Battery Energy Y: %d J\r\n", BATTERY_ENERGY);
    Serial.printf("Energy per Cycle: %.4f J\r\n", E_per_cycle/1000.0);
    Serial.printf("Total Cycles: %.1f periods\r\n", cycles);
    Serial.printf("The time of sensor node can last: %.2f hours\r\n", 
                  lifetime_hours);
    Serial.println("------------------------------------------------\r\n");
  }

  // Prepare for deep sleep by setting the wake-up time
  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME * 1000);  // unit: microsecond
  esp_deep_sleep_start();  // Start deep sleep
}

void loop() {
  // Empty loop...
}
