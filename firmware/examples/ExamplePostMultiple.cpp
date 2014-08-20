#include "M2XStreamClient/M2XStreamClient.h"

char feedId[] = "<feed id>";          // Feed you want to post to
char m2xKey[] = "<M2X access key>";   // Your M2X access key

TCPClient client;
M2XStreamClient m2xClient(&client, m2xKey);

const char *streamNames[] = { "temperature", "humidity" };
int counts[] = { 2, 1 };
const char *ats[] = { "2013-09-09T19:15:00Z",
                      "2013-09-09T19:15:10Z",
                      "2013-09-09T19:15:20Z"};
double values[] = { 10.0, 20.0, 7.5 };

void setup() {
  Serial.begin(9600);

  while (!WiFi.ready()) {
    Serial.println("Waiting for WIFI connection...");
    // wait 2 seconds for connection
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
}

void loop() {
  int response = m2xClient.postMultiple(feedId, 2, streamNames,
                                        counts, ats, values);
  Serial.print("M2x client response code: ");
  Serial.println(response);

  if (response == -1) while(1) ;

  delay(5000);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
