#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
// 連線設定( SSID 為本地wifi名稱 password為WIFI密碼 mqtt_server填寫需連線的主機端)

const char* ssid = "Lee";
const char* password = "00000000";
const char* mqtt_server = "broker.mqtt-dashboard.com";
///


//下列此段為WIFI設定流程(可不管)
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//到此上面為 WIFI 設定流程可不管








///以下為設置 馬達角為、LED_PIN燈腳位
Servo myservo;
int ledPin = D1;
int ledPin1 = D2;

//設定 MQTT 收到 指令 " 1 " 時，及為開門反之收到 " 0 " 則為關門
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);payload;
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin1, LOW); 
    myservo.write(120);
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin1,HIGH);
    myservo.write(0);
  }

}





//MQTT重新連接
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopicJIN", "hello world");
      // ... and resubscribe
      client.subscribe("inTopicJIN");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//設定觸發腳位的針腳
void setup() {
  myservo.attach(2);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
//不斷重新回圈
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, ".", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}
