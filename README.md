# linking-m5stack-natureremo

## 概要

自宅の Nature Remo から、30秒ごとに温度と湿度を取得して M5Stack に表示する

## 事前準備

WiFiのSSIDとパスワード、Nature Remo APIのアクセストークンをNVSから取得するため、事前に保存しておく必要がある

```
#include <Preferences.h>

Preferences preferences;

void setup() {
    preferences.begin("wifi");
    preferences.putString("ssid"    , "XXXXXXX");
    preferences.putString("password", "XXXXXXX");
    preferences.end();

    preferences.begin("nature-remo");
    preferences.putString("token"    , "XXXXXXX");
    preferences.end();
}

void loop() {
}
```