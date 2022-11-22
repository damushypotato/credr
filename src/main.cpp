#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 20, 4);

enum State
{
    STOP,
    READ
};

State state = STOP;

void setup()
{
    lcd.init();
    lcd.backlight();
    lcd.print("Connecting...");

    Serial.begin(57600);
    while (!Serial)
        ;

    Serial.println("READY:CREDR");
    Serial.setTimeout(10);

    SPI.begin();
    mfrc522.PCD_Init();

    lcd.clear();
}

void loop()
{
    if (Serial.available())
    {
        String data = Serial.readString();

        if (data == "STATE:READ")
        {
            state = READ;
            Serial.println("STATE=READ");
        }
        else if (data == "STATE:STOP")
        {
            state = STOP;
            Serial.println("STATE=STOP");
        }
        else if (data.substring(0, 6) == "WRITE:")
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(data.substring(6, 22));
            lcd.setCursor(0, 1);
            lcd.print(data.substring(22, 38));

            Serial.println("WRITE:OK");
        }
    }

    if (state != READ)
        return;

    if (!mfrc522.PICC_IsNewCardPresent())
        return;

    if (!mfrc522.PICC_ReadCardSerial())
        return;

    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    String uid = "SCAN:" + content.substring(1);

    Serial.println(uid);
}
