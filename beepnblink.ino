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
                // analogWrite(10, 255);
                digitalWrite(10, LOW);
                ledcWriteTone(BUZZER_CHANNEL, tmpNOTE);
                if (countblink > nblinking)
                {
                    blinking = false;
                    // analogWrite(10, 0);
                    digitalWrite(10, HIGH);
                    countblink = 0;
                    ledcWrite(BUZZER_CHANNEL, 0);
                }
            }
        }
        if (angka == endblink)
        {
            if (blinking)
            {
                // analogWrite(10, 0);
                digitalWrite(10, HIGH);
                ledcWrite(BUZZER_CHANNEL, 0);
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
            ledcWrite(BUZZER_CHANNEL, 0);

            angka = 0;
        }
        prevmill2 = millis();
    }
}