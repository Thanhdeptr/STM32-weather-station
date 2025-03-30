#include <ArduinoJson.h>
#include <ESP8266WiFi.h>



const char* ssid="The Coffee House";
const char* password="thecoffeehouse";
String apikey = "mGvVGv2gkFWY4B5fxcSzroLGyguAp8Kd";
String citycode = "4-353981_1_AL";
unsigned long timex;
char txt[3000];
String line;
int i;

WiFiClient.client;

void getWeather() {
    Serial.println("\nStarting connection to server... " + String(server));
    
    if (client.connect(server, 80)) {
        Serial.println("Connected to server\n");
        client.print("GET http://dataservice.accuweather.com/forecasts/v1/daily/1day/" + citycode + "?apikey=" + apikey + " HTTP/1.0\r\n");
        client.println("Connection: close\r\n");
        client.println();
        delay(10);
    } else {
        Serial.println("Unable to connect");
    }

    while (client.connected()) {
        String line = client.readString();
        // Serial.println(line); // Uncomment to print each line
        filter();
    }
}

void filter() {
    DynamicJsonBuffer jsonBuffer(line.length() + 1); // Tạo bộ đệm JSON động
    line.toCharArray(txt, line.length() + 1);       // Chuyển từ String sang char[] để xử lý dữ liệu

    JsonObject& root = jsonBuffer.parseObject(txt); // Phân tích chuỗi JSON thành đối tượng
    float temp_max = root["DailyForecasts"][0]["Temperature"]["Maximum"]["Value"]; // Lấy nhiệt độ tối đa
    int temp_min = root["DailyForecasts"][0]["Temperature"]["Minimum"]["Value"];   // Lấy nhiệt độ tối thiểu
    String wea = root["DailyForecasts"][0]["Day"]["IconPhrase"];                   // Lấy mô tả thời tiết

    Serial.print("\n");
    if (temp_max != 0) {
        Serial.println("Temp_max: " + String(temp_max) + "°C");
        Serial.println(" ");
        Serial.println("Temp_min: " + String(temp_min) + "°C");
        Serial.println("Weather: " + wea);
        Serial.println("\n");
    }
}


void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
Serial.println("wifi connecting to: ");
Serial.print(ssid);
WiFi.begin(ssid,password);
Serial.println("Connecting ");
while(WiFi.status() != WL_CONNECTED){
  delay(200);
  Serial.print(".");
}
Serial.println("Connect success");
Serial.println("esp address is: ");
Serial.print(WiFi.localIP());

}

void loop() {
  if(millis()- timex>1000);
  getWeather();
  timex=millis();

}
