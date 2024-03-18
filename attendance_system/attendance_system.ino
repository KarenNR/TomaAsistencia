/*
FINGERPRINT SENSOR CONNECTIONS
* Red cable - 3.3V
* Black cable - GND
* Yellow cable - 0
* White cable - 1

RELAY
* Yellow cable - 4
* Red cable - 5V
* Black cable - GND
*/

#include <Adafruit_Fingerprint.h>
#include "WiFiS3.h"
#include <ArduinoHttpClient.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 3);

#else
#define mySerial Serial1

#endif

#define RELAY_PIN       4

#define ACCESS_DELAY    5000 

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

/* --- WIFI SETUP --- */
char ssid[] = "Recamara Karen";      
char pass[] = "kgt150202?";  
int keyIndex = 0;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

WiFiClient wifi;
HttpClient client = HttpClient(wifi, "192.168.3.2", 3000);

void setup()
{
  Serial.begin(9600);
  delay(1000);
  while (!Serial);

  /* --- WIFI CONNECTION --- */

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Error en la comunicación con el módulo WiFi.");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Por favor actualice el firmware.");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.println("Conectando a WiFi...");
    Serial.print("Nombre de la red: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2 network.
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();

  /* --- FINGERPRINT SENSOR --- */

  Serial.println("\n\nDatos del sensor de huellas Adafruit");
  
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("¡Se ha encontrado el sensor de huellas!");
  } else {
    Serial.println("No se ha encontrado el sensor de huellas :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("No existen huellas registradas en el sensor.");
  }
  else {
    Serial.println("Esperando huella...");
      Serial.print("El sensor contiene "); Serial.print(finger.templateCount); Serial.println(" plantillas");
  }

  /* --- LOCK --- */
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 
}

void loop()
{
  getFingerprintID();
  delay(50);
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen tomada");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error de imagen");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen confusa");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No se pudieron encontrar funciones de huellas dactilares");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No se pudieron encontrar funciones de huellas dactilares");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("¡Coincidencia encontrada!");
    // Connect to database
    makePost(finger.fingerID);
    // Open door
    Serial.println("Abriendo puerta para el alumno...");
    digitalWrite(RELAY_PIN, HIGH);
    delay(ACCESS_DELAY);
    digitalWrite(RELAY_PIN, LOW);   
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("No se encontró una coincidencia");
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }

  // found a match!
  Serial.println("¡Asistencia registrada!");
  Serial.print("Se encontró el ID #"); Serial.print(finger.fingerID);
  Serial.print(" con una confianza de "); Serial.println(finger.confidence);
  Seria.println("");

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Se encontró el ID #"); Serial.print(finger.fingerID);
  Serial.print(" con una confianza de "); Serial.println(finger.confidence);
  return finger.fingerID;
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void makePost(int fingerId) {
  Serial.println("Haciendo petición POST para registrar asistencia en la base de datos...");
  String contentType = "application/x-www-form-urlencoded";
  String postData = "id=" + String(fingerId);

  client.post("/", contentType, postData);

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}
