#include "display.hpp"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

TaskHandle_t DisplayTask;

const unsigned int SLEEP_AFTER_MS = 60 * 1000; // sleep after 60 seconds
bool dispAsleep = false;

void CenterPrintToScreen(char const *str, u8g2_uint_t y) {
  u8g2_uint_t width = u8g2.getStrWidth(str);
  u8g2.setCursor(128 / 2 - width / 2, y);
  u8g2.print(str);
}

void LeftPrintToScreen(char const *str, u8g2_uint_t y)
{
  u8g2_uint_t width = u8g2.getStrWidth(str);
  u8g2.setCursor(5, y);
  u8g2.print(str);
}

void LeftPrintActiveToScreen(char const *str, u8g2_uint_t y)
{
  u8g2_uint_t width = u8g2.getStrWidth(str);
  u8g2.setDrawColor(1);
  u8g2.drawBox(3, y - 1, 122, 14);
  u8g2.setDrawColor(0);
  u8g2.setCursor(5, y);
  u8g2.print(str);
  u8g2.setDrawColor(1);
}

void RightPrintToScreen(char const *str, u8g2_uint_t y)
{
  u8g2_uint_t width = u8g2.getStrWidth(str);
  u8g2.setCursor(123-width, y);
  u8g2.print(str);
}

void showMenu(){
  int prevIndex = (currentMenuItem - 1) % menuItemsCount;
  int nextIndex = (currentMenuItem + 1) % menuItemsCount;

  prevIndex = prevIndex < 0 ? prevIndex + menuItemsCount : prevIndex;
  MenuItem prev = menuItems[prevIndex];
  MenuItem current = menuItems[currentMenuItem];
  MenuItem next = menuItems[nextIndex];
  char buf[3];
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x14B_tf);
  CenterPrintToScreen("Menu", 0);
  u8g2.setFont(u8g2_font_7x13_tr);
  LeftPrintToScreen(prev.menuName, 19);
  LeftPrintActiveToScreen(current.menuName, 35);
  LeftPrintToScreen(next.menuName, 51);

  u8g2.sendBuffer();
}

void showOffsetMenu(){
  char buf[16];
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x14B_tf);
  CenterPrintToScreen("Adjust Offset", 35);
  u8g2.setFont(u8g2_font_7x13_tr);
  snprintf(buf, sizeof(buf), "%3.2fg", offset);
  CenterPrintToScreen(buf, 51);
  u8g2.sendBuffer();
}


void showManualGrindMenu(){
  char buf[16];
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x13_tr);
  CenterPrintToScreen("Click to start", 35);
  u8g2.setFont(u8g2_font_7x13_tr);
  CenterPrintToScreen("grinding", 51);
  u8g2.sendBuffer();
}


void showScaleModeMenu()
{
  char buf[16];
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x14B_tf);
  CenterPrintToScreen("Scale Mode", 19);
  u8g2.setFont(u8g2_font_7x13_tr);
  if(scaleMode){
    LeftPrintToScreen("GBW", 35);
    LeftPrintActiveToScreen("Scale only", 51);
  }
  else{
    LeftPrintActiveToScreen("GBW", 35);
    LeftPrintToScreen("Scale only", 51);
  }
  u8g2.sendBuffer();
}

void showGrindModeMenu()
{
  char buf[16];
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x14B_tf);
  CenterPrintToScreen("Run Mode", 19);
  u8g2.setFont(u8g2_font_7x13_tr);
  if (grindMode)
  {
    LeftPrintActiveToScreen("Continuous", 35);
    LeftPrintToScreen("Impulse", 51);
  }
  else
  {
    LeftPrintToScreen("Continuous", 35);
    LeftPrintActiveToScreen("Impulse", 51);
  }
  u8g2.sendBuffer();
}

void showCupMenu()
{
  char buf[16];
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x14B_tf);
  CenterPrintToScreen("Cup Weight", 0);
  u8g2.setFont(u8g2_font_7x13_tr);
  snprintf(buf, sizeof(buf), "%3.1fg", scaleWeight);
  CenterPrintToScreen(buf, 19);
  LeftPrintToScreen("Place cup on scale", 35);
  LeftPrintToScreen("and press button", 51);
  u8g2.sendBuffer();
}

void showCalibrationMenu(){
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x14B_tf);
  CenterPrintToScreen("Calibration", 0);
  u8g2.setFont(u8g2_font_7x13_tr);
  CenterPrintToScreen("Place 100g weight", 19);
  CenterPrintToScreen("on scale and", 35);
  CenterPrintToScreen("press button", 51);
  u8g2.sendBuffer();
}

void showResetMenu()
{
  char buf[16];
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x14B_tf);
  CenterPrintToScreen("Reset to defaults?", 0);
  u8g2.setFont(u8g2_font_7x13_tr);
  if (greset)
  {
    LeftPrintActiveToScreen("Confirm", 19);
    LeftPrintToScreen("Cancel", 35);
  }
  else
  {
    LeftPrintToScreen("Confirm", 19);
    LeftPrintActiveToScreen("Cancel", 35);
  }
  u8g2.sendBuffer();
}

void showTimeoutMenu(){
  char buf[16];
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x14B_tf);
  CenterPrintToScreen("Grinder Timeout", 35);
  u8g2.setFont(u8g2_font_7x13_tr);
  snprintf(buf, sizeof(buf), "%1u sec", grinderTimeout/1000);
  CenterPrintToScreen(buf, 51);
  u8g2.sendBuffer();
}

void showGrindRateMenu(){
  char buf[16];
  u8g2.clearBuffer();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_7x14B_tf);
  CenterPrintToScreen("Grind Rate", 35);
  u8g2.setFont(u8g2_font_7x13_tr);
  snprintf(buf, sizeof(buf), "%2.1f g/sec", grindRate);
  CenterPrintToScreen(buf, 51);
  u8g2.sendBuffer();
}

void showSetting(){
  if(currentSetting == 3){
    showOffsetMenu();
  }
  else if(currentSetting == 0){
    showManualGrindMenu();
  }
  else if(currentSetting == 1){
    showCupMenu();
  }
  else if (currentSetting == 2)
  {
    showCalibrationMenu();
  }
  else if (currentSetting == 4)
  {
    showScaleModeMenu();
  }
  else if (currentSetting == 5)
  {
    showGrindModeMenu();
  }
  else if (currentSetting == 7)
  {
    showResetMenu();
  }
  else if (currentSetting == 9)
  {
    showTimeoutMenu();
  }
  else if (currentSetting == 10)
  {
    showGrindRateMenu();
  }
}


void updateDisplay( void * parameter) {
  char buf[64];
  char buf2[64];

  for(;;) {
    u8g2.clearBuffer();

    if (dispAsleep && !wakeDisp)
      continue;

    if (!wakeDisp
        && (millis() - lastAction) > SLEEP_AFTER_MS) {
      u8g2.sendBuffer();
      delay(100);
      dispAsleep = true;
      continue;
    }

    dispAsleep = false;

    if (scaleLastUpdatedAt == 0) {
      u8g2.setFontPosTop();
      u8g2.drawStr(0, 20, "Initializing...");
    } else if (!scaleReady) {
      u8g2.setFontPosTop();
      u8g2.drawStr(0, 20, "SCALE ERROR");
    } else {
      if (scaleStatus == STATUS_GRINDING_IN_PROGRESS) {
        u8g2.setFontPosTop();
        u8g2.setFont(u8g2_font_7x13_tr);
        CenterPrintToScreen("Grinding...", 0);


        u8g2.setFontPosCenter();
        u8g2.setFont(u8g2_font_7x14B_tf);
        u8g2.setCursor(3, 32);
        snprintf(buf, sizeof(buf), "%3.1fg", scaleWeight - cupWeightEmpty);
        u8g2.print(buf);

        u8g2.setFontPosCenter();
        u8g2.setFont(u8g2_font_unifont_t_symbols);
        u8g2.drawGlyph(64, 32, 0x2794);

        u8g2.setFontPosCenter();
        u8g2.setFont(u8g2_font_7x14B_tf);
        u8g2.setCursor(84, 32);
        snprintf(buf, sizeof(buf), "%3.1fg", setWeight);
        u8g2.print(buf);

        u8g2.setFontPosBottom();
        u8g2.setFont(u8g2_font_7x13_tr);
        snprintf(buf, sizeof(buf), "%3.1fs", startedGrindingAt > 0 ? (double)(millis() - startedGrindingAt) / 1000 : 0);
        CenterPrintToScreen(buf, 64);
      } else if (scaleStatus == STATUS_EMPTY) {
        u8g2.setFontPosTop();
        u8g2.setFont(u8g2_font_7x13_tr);
        CenterPrintToScreen("Weight:", 0);

        u8g2.setFont(u8g2_font_7x14B_tf);
        u8g2.setFontPosCenter();
        u8g2.setCursor(0, 28);
        snprintf(buf, sizeof(buf), "%3.1fg", abs(scaleWeight));
        CenterPrintToScreen(buf, 32);

        u8g2.setFont(u8g2_font_7x13_tf);
        u8g2.setFontPosCenter();
        u8g2.setCursor(0, 50);
        snprintf(buf2, sizeof(buf2), "Set: %3.1fg", setWeight);
        CenterPrintToScreen(buf2, 50);

        
      } else if (scaleStatus == STATUS_GRINDING_FAILED) {

        u8g2.setFontPosTop();
        u8g2.setFont(u8g2_font_7x14B_tf);
        CenterPrintToScreen("Grinding failed", 0);

        u8g2.setFontPosTop();
        u8g2.setFont(u8g2_font_7x13_tr);
        CenterPrintToScreen("Press the balance", 32);
        CenterPrintToScreen("to reset", 42);
      } else if (scaleStatus == STATUS_GRINDING_FINISHED) {

        u8g2.setFontPosTop();
        u8g2.setFont(u8g2_font_7x13_tr);
        u8g2.setCursor(0, 0);
        CenterPrintToScreen("Grinding finished", 0);

        u8g2.setFontPosCenter();
        u8g2.setFont(u8g2_font_7x14B_tf);
        u8g2.setCursor(3, 32);
        snprintf(buf, sizeof(buf), "%3.1fg", scaleWeight - cupWeightEmpty);
        u8g2.print(buf);

        u8g2.setFontPosCenter();
        u8g2.setFont(u8g2_font_unifont_t_symbols);
        u8g2.drawGlyph(64, 32, 0x2794);

        u8g2.setFontPosCenter();
        u8g2.setFont(u8g2_font_7x14B_tf);
        u8g2.setCursor(84, 32);
        snprintf(buf, sizeof(buf), "%3.1fg", setWeight);
        u8g2.print(buf);

        u8g2.setFontPosBottom();
        u8g2.setFont(u8g2_font_7x13_tr);
        u8g2.setCursor(64, 64);
        snprintf(buf, sizeof(buf), "%3.1fs", (double)(finishedGrindingAt - startedGrindingAt) / 1000);
        CenterPrintToScreen(buf, 64);
      }
      else if (scaleStatus == STATUS_IN_MENU)
      {
        showMenu();
      }
      else if (scaleStatus == STATUS_IN_SUBMENU)
      {
        showSetting();
      }
      else if (scaleStatus == STATUS_TARING)
      {
        char buf[16];
        u8g2.clearBuffer();
        u8g2.setFontPosTop();
        u8g2.setFont(u8g2_font_7x13_tr);
        CenterPrintToScreen("Taring...", 20);
        u8g2.setFont(u8g2_font_7x13_tr);
        CenterPrintToScreen("Remove weight", 38);
        u8g2.sendBuffer();
      }
    }
    u8g2.sendBuffer();
    // delay(100);
  }
}

void setupDisplay() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_7x13_tr);
  u8g2.setFontPosTop();
  u8g2.drawStr(0, 20, "Hello");

  xTaskCreatePinnedToCore(
      updateDisplay, /* Function to implement the task */
      "Display", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &DisplayTask,  /* Task handle. */
      1); /* Core where the task should run */
}
