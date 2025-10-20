void displayClock()
{
    tft.fillScreen(TFT_BLACK);
    // tft.setCursor(random(0, 20), random(20, 60));
    String st = String(hour());
    st += ":";
    st += String(minute());

    // printtextcs(st, );
    printtextcs(random(0, 20), random(20, 60), st, COLOR_MEDIUM[random(12)], 16);
}