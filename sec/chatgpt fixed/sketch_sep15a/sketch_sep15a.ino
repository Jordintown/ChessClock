#include <SPI.h>
#include <Wire.h>
#include <ss_oled.h>

#define GROVE_SDA_PIN A2
#define GROVE_SCL_PIN A3
#define SDA_PIN A4
#define SCL_PIN A5
#define RESET_PIN -1
#define OLED_ADDR -1
#define FLIP180 0
#define INVERT 0
#define USE_HW_I2C 0
#define MY_OLED1 OLED_128x64
#define MY_OLED2 OLED_128x64

#define PAUSE 3

SSOLED ssoled[2];

unsigned long clock, lastClock;
unsigned long secondsLeft[2];
unsigned long bonusSeconds, bonusMinutes; // Separate bonus time variables
int moves[2];     // Move counters
int player = PAUSE;
bool buttonPressed[2];
unsigned long prevDisplay[2] = {0, 0}; // Previous display values for time

void setup() {
    Serial.begin(9600);
    int initStatus;

    initStatus = oledInit(&ssoled[0], MY_OLED1, OLED_ADDR, FLIP180, INVERT, 1, SDA_PIN, SCL_PIN, RESET_PIN, 400000L);
    initStatus = oledInit(&ssoled[1], MY_OLED2, OLED_ADDR, FLIP180, INVERT, 0, GROVE_SDA_PIN, GROVE_SCL_PIN, RESET_PIN, 400000L);
  
    for (int i = 0; i < 2; i++) {
        secondsLeft[i] = 900; // Initialize with 15 minutes (900 seconds)
        moves[i] = 0;
        buttonPressed[i] = false;
        oledFill(&ssoled[i], 0, 1);
    }
  
    bonusSeconds = 5; // Initialize bonus seconds
    bonusMinutes = 0; // Initialize bonus minutes
    lastClock = 0;

    pinMode(2, INPUT_PULLUP); // Player 1 button
    pinMode(3, INPUT_PULLUP); // Player 2 button
    pinMode(4, INPUT_PULLUP); // For increasing
    pinMode(5, INPUT_PULLUP); // For confirm
    pinMode(6, INPUT_PULLUP); // For decreasing
    
    timeSetting(1);
}

void timeSetting(int type) {
    char buf[15];
    unsigned long *timeSettingVar;  // Pointer to the time setting variable
    int timeSettingStage = 0; // Track which part of the time is being set
    bool adjustingTime = true;  // Flag to indicate time setting mode

    while (adjustingTime) {
        oledFill(&ssoled[0], 0, 1);
        oledWriteString(&ssoled[0], 0, 0, 0, (char *)"Time Settings", FONT_SMALL, 0, 1);
        oledWriteString(&ssoled[0], 0, 0, 1, (char *)"Press 5 to confirm", FONT_SMALL, 0, 1);
        oledWriteString(&ssoled[0], 0, 0, 2, (char *)"Press 4 to change", FONT_SMALL, 0, 1);
        oledWriteString(&ssoled[0], 0, 0, 3, (char *)"Press 6 to decrease", FONT_SMALL, 0, 1);

        // Show current time and bonus values based on the current setting stage
        switch (timeSettingStage) {
            case 0: // Setting seconds
                oledWriteString(&ssoled[0], 0, 0, 4, (char *)"Set Seconds", FONT_SMALL, 0, 1);
                ToHMS(secondsLeft[0], buf);
                oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 0, 1);
                break;
            case 1: // Setting minutes
                oledWriteString(&ssoled[0], 0, 0, 4, (char *)"Set Minutes", FONT_SMALL, 0, 1);
                ToHMS(secondsLeft[0] + 3600, buf); // Displaying as hours and minutes
                oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 0, 1);
                break;
            case 2: // Setting hours
                oledWriteString(&ssoled[0], 0, 0, 4, (char *)"Set Hours", FONT_SMALL, 0, 1);
                ToHMS(secondsLeft[0] + 7200, buf); // Displaying as hours and minutes
                oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 0, 1);
                break;
            case 3: // Setting bonus seconds
                oledWriteString(&ssoled[0], 0, 0, 4, (char *)"Set Bonus Sec", FONT_SMALL, 0, 1);
                ToHMS(bonusSeconds, buf);
                oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 0, 1);
                break;
            case 4: // Setting bonus minutes
                oledWriteString(&ssoled[0], 0, 0, 4, (char *)"Set Bonus Min", FONT_SMALL, 0, 1);
                ToHMS(bonusMinutes * 60, buf); // Displaying bonus minutes in seconds
                oledWriteString(&ssoled[1], 0, 0, 2, (char *)buf, FONT_STRETCHED, 0, 1);
                break;
        }

        if (digitalRead(5) == LOW) {  // Confirm setting
            if (timeSettingStage < 4) {
                timeSettingStage++; // Move to the next stage
            } else {
                adjustingTime = false;  // Exit the loop and save changes
                Serial.println("Settings saved.");
                oledWriteString(&ssoled[0], 0, 0, 5, (char *)"Settings Saved", FONT_SMALL, 0, 1);
                delay(2000); // Show confirmation for 2 seconds
            }
            delay(200); // Debounce delay
        }
        
        if (digitalRead(4) == LOW) {  // Increase
            switch (timeSettingStage) {
                case 0:
                    secondsLeft[0] = min(59UL, secondsLeft[0] + 1); // Limit to 59 seconds
                    break;
                case 1:
                    secondsLeft[0] = min(59UL, (secondsLeft[0] / 60) + 1) * 60; // Limit to 59 minutes
                    break;
                case 2:
                    secondsLeft[0] += 3600; // Increase by one hour
                    break;
                case 3:
                    bonusSeconds = min(59UL, bonusSeconds + 1); // Limit to 59 seconds
                    break;
                case 4:
                    bonusMinutes = min(59UL, bonusMinutes + 1); // Limit to 59 minutes
                    break;
            }
            delay(200);  // Debounce delay
        }

        if (digitalRead(6) == LOW) {  // Decrease
            switch (timeSettingStage) {
                case 0:
                    secondsLeft[0] = max(0UL, secondsLeft[0] - 1); // Decrease time
                    break;
                case 1:
                    secondsLeft[0] = max(0UL, (secondsLeft[0] / 60) - 1) * 60; // Decrease time
                    break;
                case 2:
                    if (secondsLeft[0] >= 3600) {
                        secondsLeft[0] -= 3600; // Decrease by one hour
                    }
                    break;
                case 3:
                    bonusSeconds = max(0UL, bonusSeconds - 1); // Decrease bonus seconds
                    break;
                case 4:
                    bonusMinutes = max(0UL, bonusMinutes - 1); // Decrease bonus minutes
                    break;
            }
            delay(200);  // Debounce delay
        }

        if (digitalRead(4) == LOW || digitalRead(6) == LOW) { // Cancel settings
            adjustingTime = false;  // Exit the loop
            Serial.println("Settings canceled.");
            oledWriteString(&ssoled[0], 0, 0, 5, (char *)"Settings Canceled", FONT_SMALL, 0, 1);
            delay(2000); // Show cancellation for 2 seconds
        }
    }

    oledFill(&ssoled[0], 0, 1);  // Clear the screen after setting
    oledFill(&ssoled[1], 0, 1);
}

char* ToHMS(unsigned long seg, char *buffer) {
    int hours = seg / 3600;
    int minutes = (seg % 3600) / 60;
    int seconds = seg % 60;
    sprintf(buffer, "%2d:%02d:%02d   ", hours, minutes, seconds);
    return buffer;
}

// Function to display remaining time
void mostrarTiempoRestante(SSOLED *oled, unsigned long segundosRestantes, unsigned long &prevDisplay) {
    if (segundosRestantes != prevDisplay) {
        char buffer[15];
        ToHMS(segundosRestantes, buffer);
        oledWriteString(oled, 0, 0, 3, buffer, FONT_STRETCHED, 0, 1);
        prevDisplay = segundosRestantes;  // Update previous display value
    }
}

// Function to refresh the display
void refrescaDisplay(SSOLED *pantalla, unsigned long segunds, unsigned long &prevDisplay) {
    oledFill(pantalla, 0, 1);  // Clear the display first
    mostrarTiempoRestante(pantalla, segunds, prevDisplay);  // Pass prevDisplay by reference
}

// Main loop function
void loop() {
    clock = millis();

    if (clock > (lastClock + 999)) { // Update every second
        lastClock = clock;

        switch (player) {
            case 0:
                if (secondsLeft[0] > 0) {
                    secondsLeft[0]--;
                }
                break;

            case 1:
                if (secondsLeft[1] > 0) {
                    secondsLeft[1]--;
                }
                break;

            case PAUSE:
                break;
        }

        // Refresh the display for both players
        for (int i = 0; i < 2; i++) {
            refrescaDisplay(&ssoled[i], secondsLeft[i], prevDisplay[i]); // Correct call
        }
    }

    // Detecting player buttons
    if (digitalRead(2) == LOW && !buttonPressed[0] && (player == 0 || player == PAUSE)) {  
        player = 1;  // Change to Player 2
        secondsLeft[0] += bonusSeconds + (bonusMinutes * 60); // Add bonus to Player 2
        moves[0]++;  // Increment Player 1 moves
        buttonPressed[0] = true;
        Serial.println("Player 1 pressed.");
    }

    if (digitalRead(3) == LOW && !buttonPressed[1] && (player == 1 || player == PAUSE)) {
        player = 0;  // Change to Player 1
        secondsLeft[1] += bonusSeconds + (bonusMinutes * 60); // Add bonus to Player 1
        moves[1]++;  // Increment Player 2 moves
        buttonPressed[1] = true;
        Serial.println("Player 2 pressed.");
    }

    // Button release detection for Player 1
    if (digitalRead(2) == HIGH) {
        buttonPressed[0] = false;  // Reset button press flag for Player 1
    }

    // Button release detection for Player 2
    if (digitalRead(3) == HIGH) {
        buttonPressed[1] = false;  // Reset button press flag for Player 2
    }
}
