#include <M5Stack.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct {
    String wifiSsid;
    String wifiPassword;
    String natureRemoToken;
}nvsData;

void setup() {
    M5.begin();
    M5.Power.begin();

    // NVSに格納されているデータの取得
    nvsData.wifiSsid        = retrieveNvsData("wifi"       , "ssid");
    nvsData.wifiPassword    = retrieveNvsData("wifi"       , "password");
    nvsData.natureRemoToken = retrieveNvsData("nature-remo", "token");

    // WiFi接続
    connectWifi(nvsData.wifiSsid, nvsData.wifiPassword);
}

void loop() {
    // Nature Remo にアクセス
    String natureRemoInformation = accessNatureRemoInformation(nvsData.natureRemoToken);

    // JSONパース
    JsonObject newest_events = parseJson(natureRemoInformation);

    // 室温と湿度を取得
    float temperature = newest_events["te"]["val"];
    int   humidity    = newest_events["hu"]["val"];

    // 画面に表示する
    displayOnScreen(temperature, humidity);

    // 30秒間隔
    delay(30000);
}

/**
 * NVSに格納されているデータを取得する
 */
String retrieveNvsData(String nameSpace, String key) {
    Preferences preferences;
    bool isReadOnly = true;
    preferences.begin(nameSpace.c_str(), isReadOnly);
    String value = preferences.getString(key.c_str());
    preferences.end();

    return value;
}

/**
 * WiFi接続
 */
void connectWifi(const String &wifiSsid, const String &wifiPassword) {
    M5.Lcd.print("Connecting to WiFi");
    WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());
    // 接続できるまで最大30秒間試行
    for (int i = 0; WiFi.status() != WL_CONNECTED; i++) {
        if (i >= 30) {
            // 接続できない場合はリセット
            M5.Power.reset();
        }
        delay(1000);
        M5.Lcd.print(".");
    }
}

/**
 * Nature Remo の情報を取得する
 */
String accessNatureRemoInformation(const String &natureRemoToken) {
    HTTPClient http;
    http.begin("https://api.nature.global/1/devices");
    http.addHeader("accept", "application/json");
    http.addHeader("Authorization", "Bearer " + natureRemoToken);
    int retVal = http.GET();
    String response = http.getString();
    http.end();

    return response;
}

/**
 * JSONパース
 * https://arduinojson.org/v6/assistant/ で作られたサンプルコードをもとに記載
 */
JsonObject parseJson(const String &natureRemoInformation) {
    StaticJsonDocument<1024> doc;

    DeserializationError error = deserializeJson(doc, natureRemoInformation);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        // エラーならリセット
        M5.Power.reset();
    }

    return doc[0]["newest_events"];
}

/**
 * 画面表示
 */
void displayOnScreen(const float &temperature, const int &humidity) {
    // 画面クリア
    ClearScreen();

    // 室温表示
    displayTemperature(temperature);

    // 行間表示
    displayLineSpacing();

    // 湿度表示
    displayHumidity(humidity);
}

/**
 * 画面クリア
 */
void ClearScreen() {
    M5.Lcd.clear(BLACK);
    M5.Lcd.setCursor(0, 0);
}

/**
 * 室温表示
 */
void displayTemperature(const float &temperature) {
    displayItemName("temperature");
    displayLineSpacing();
    displayValueTemperature(temperature);
}

/**
 * 湿度表示
 */
void displayHumidity(const int &humidity) {
    displayItemName("humidity");
    displayLineSpacing();
    displayValueHumidity(humidity);
}

/**
 * 項目名表示
 */
void displayItemName(const String &itemName) {
    M5.Lcd.setTextSize(4);
    M5.Lcd.println(itemName);
}

/**
 * 行間表示
 */
void displayLineSpacing() {
    M5.Lcd.setTextSize(2);
    M5.Lcd.println();
}

/**
 * 値表示(室温)
 */
void displayValueTemperature(const float &temperature) {
    M5.Lcd.setTextSize(10);
    M5.Lcd.printf(" %.1f\n", temperature);
}

/**
 * 値表示(湿度)
 */
void displayValueHumidity(const int &humidity) {
    M5.Lcd.setTextSize(10);
    M5.Lcd.printf(" %.d\n", humidity);
}