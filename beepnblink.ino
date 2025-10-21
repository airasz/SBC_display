void beepnblink2()
{
    if (millis() > prevmill2 + 100)
    {
        angka++;
        if (angka == startblink)
        {
            if (blinking)
            {
                countblink++;
                // analogWrite(16, 255);
                NEO.setPixelColor(0, NEO.Color(0, 170, 0));
                NEO.show();
                // ledcWriteTone(BUZZER_CHANNEL, tmpNOTE);
                tone(BUZZER_PIN, tmpNOTE);
                Serial.println("beep " + String(countblink));
                if (countblink > nblinking)
                {
                    blinking = false;
                    analogWrite(16, 0);
                    countblink = 0;
                    NEO.setPixelColor(0, 0);
                    NEO.show();
                    // ledcWrite(BUZZER_CHANNEL, 0);
                    noTone(BUZZER_PIN);
                }
            }
        }
        if (angka == endblink)
        {
            if (blinking)
            {
                NEO.setPixelColor(0, 0);
                NEO.show();
                // analogWrite(16, 0);
                // ledcWrite(BUZZER_CHANNEL, 0);
                noTone(BUZZER_PIN);
                if (endmatch == 2)
                {
                    Serial.println("endmatch == 2");
                    endmatch = 0;
                    endblink = 9;
                }
                else if (endmatch == 1)
                {
                    endblink = 19;
                    startblink = 9;
                    endmatch = 2;
                }
            }

            angka = 0;
        }
        prevmill2 = millis();
    }
}
void beepnblink()
{
    if (millis() > prevmill2 + 100)
    {
        angka++;
        if (angka == startblink)
        {
            if (blinking)
            {
                countblink++;
                if (countblink <= nblinking)
                {
                    // digitalWrite(8, LOW);
                    // ledcWriteTone(BUZZER_CHANNEL, tmpNOTE);
                    NEO.setPixelColor(0, NEO.Color(0, 170, 0));
                    NEO.show();
                    tone(BUZZER_PIN, tmpNOTE);
                    Serial.println("beep");
                }
                else
                {
                    blinking = false;
                    NEO.setPixelColor(0, NEO.Color(0, 0, 0));
                    NEO.show();
                    noTone(BUZZER_PIN);

                    // analogWrite(12, 0);
                    countblink = 0;
                }
            }
        }
        if (angka == endblink)
        {
            if (blinking)
            {
                // analogWrite(12, 0);
                // NEO.setPixelColor(0, 0);
                // NEO.show();
                // ledcWrite(BUZZER_CHANNEL, 0);
                // digitalWrite(8, HIGH);
                NEO.setPixelColor(0, NEO.Color(0, 0, 0));
                NEO.show();
                noTone(BUZZER_PIN);
                Serial.println("stop beep");
                if (endmatch == 2)
                {
                    Serial.println("endmatch == 2");
                    endmatch = 0;
                    endblink = 9;
                }
                else if (endmatch == 1)
                {
                    endblink = 19;
                    startblink = 9;
                    endmatch = 2;
                }
            }

            angka = 0;
        }
        prevmill2 = millis();
    }
}