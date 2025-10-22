void displayClock()
{
    tft.fillScreen(TFT_BLACK);
    // tft.setCursor(random(0, 20), random(20, 60));
    String st = "";
    if (hour() < 10)
        st += "0";
    st += String(hour());
    st += ":";
    if (minute() < 10)
        st += "0";
    st += String(minute());
    int cx = random(0, 20), cy = random(20, 80);
    // Serial.printf("cx : %d cy : %d\n", cx, cy);
    // printtextcs(st, );
    printtextcs(cx, cy, st, COLOR_MEDIUM[random(12)], 56);
}