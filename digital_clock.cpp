#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <cstring>
#include <fstream>
#include <cmath>

using namespace sf;
using namespace std;

const int WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 700;
const int MAX_ALARMS = 10, MAX_ALARM_DESC = 50;

int currentMode = 0;
bool is24Hour = true, alarmTriggered = false;
int alarmBlinkCounter = 0, lastAlarmHour = -1, lastAlarmMinute = -1;

SoundBuffer alarmSoundBuffer;
Sound alarmSound;
bool soundInitialized = false;

int alarmHours[MAX_ALARMS], alarmMinutes[MAX_ALARMS];
bool alarmEnabled[MAX_ALARMS];
char alarmDescriptions[MAX_ALARMS][MAX_ALARM_DESC];
int alarmCount = 0;

bool stopwatchRunning = false, stopwatchPaused = false;
int stopwatchHours = 0, stopwatchMinutes = 0, stopwatchSeconds = 0, stopwatchMilliseconds = 0;
Clock stopwatchClock;
Time stopwatchElapsed;

bool timerRunning = false, timerFinished = false, timerSoundPlayed = false;
int timerHours = 0, timerMinutes = 0, timerSeconds = 0, timerTotalSeconds = 0;
Clock timerClock;

bool inputMode = false;
int inputType = 0, inputValue = 0, currentAlarmIndex = 0;

void generateBeepSound(SoundBuffer& buffer) {
    const int sampleRate = 44100;
    const float duration = 0.5f, frequency = 800.0f;
    const int sampleCount = static_cast<int>(sampleRate * duration);
    short* samples = new short[sampleCount * 2];
    
    for (int i = 0; i < sampleCount; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float value = sin(2.0f * 3.14159f * frequency * t);
        float envelope = 1.0f;
        if (i < sampleRate * 0.05f) envelope = static_cast<float>(i) / (sampleRate * 0.05f);
        else if (i > sampleCount - sampleRate * 0.05f) envelope = static_cast<float>(sampleCount - i) / (sampleRate * 0.05f);
        
        short sample = static_cast<short>(value * envelope * 16000);
        samples[i * 2] = samples[i * 2 + 1] = sample;
    }
    
    buffer.loadFromSamples(samples, sampleCount * 2, 2, sampleRate);
    delete[] samples;
}

void initializeAlarmSound() {
    if (!soundInitialized) {
        generateBeepSound(alarmSoundBuffer);
        alarmSound.setBuffer(alarmSoundBuffer);
        alarmSound.setLoop(true);
        alarmSound.setVolume(70.0f);
        soundInitialized = true;
    }
}

void initializeAlarms() {
    for (int i = 0; i < MAX_ALARMS; i++) {
        alarmHours[i] = alarmMinutes[i] = 0;
        alarmEnabled[i] = false;
        strcpy(alarmDescriptions[i], "");
    }
    alarmCount = 0;
}

void saveAlarms() {
    ofstream file("alarms.txt");
    if (file.is_open()) {
        file << alarmCount << endl;
        for (int i = 0; i < alarmCount; i++)
            file << alarmHours[i] << " " << alarmMinutes[i] << " " << alarmEnabled[i] << " " << alarmDescriptions[i] << endl;
        file.close();
    }
}

void loadAlarms() {
    ifstream file("alarms.txt");
    if (file.is_open()) {
        file >> alarmCount;
        for (int i = 0; i < alarmCount && i < MAX_ALARMS; i++) {
            file >> alarmHours[i] >> alarmMinutes[i] >> alarmEnabled[i];
            file.ignore();
            file.getline(alarmDescriptions[i], MAX_ALARM_DESC);
        }
        file.close();
    }
}

void updateClock(int& h, int& m, int& s, int& d, int& mo, int& y) {
    time_t now = time(0);
    tm* t = localtime(&now);
    h = t->tm_hour; m = t->tm_min; s = t->tm_sec;
    d = t->tm_mday; mo = t->tm_mon + 1; y = t->tm_year + 1900;
}

void formatTime(int h, int m, int s, char* buf, bool is24) {
    if (is24) sprintf(buf, "%02d:%02d:%02d", h, m, s);
    else {
        int dh = h % 12;
        if (dh == 0) dh = 12;
        sprintf(buf, "%02d:%02d:%02d %s", dh, m, s, (h < 12) ? "AM" : "PM");
    }
}

void formatDate(int d, int m, int y, char* buf) {
    const char* months[] = {"January", "February", "March", "April", "May", "June",
                            "July", "August", "September", "October", "November", "December"};
    sprintf(buf, "%s %d, %d", months[m - 1], d, y);
}

void checkAlarms(int h, int m) {
    if (lastAlarmHour == h && lastAlarmMinute == m) return;
    
    for (int i = 0; i < alarmCount; i++) {
        if (alarmEnabled[i] && alarmHours[i] == h && alarmMinutes[i] == m) {
            alarmTriggered = true;
            alarmBlinkCounter = 0;
            lastAlarmHour = h;
            lastAlarmMinute = m;
            initializeAlarmSound();
            if (alarmSound.getStatus() != Sound::Playing) alarmSound.play();
            break;
        }
    }
}

void updateStopwatch() {
    if (stopwatchRunning && !stopwatchPaused) {
        stopwatchElapsed = stopwatchClock.getElapsedTime();
        int totalMs = stopwatchElapsed.asMilliseconds();
        stopwatchMilliseconds = totalMs % 1000;
        int totalSec = totalMs / 1000;
        stopwatchSeconds = totalSec % 60;
        int totalMin = totalSec / 60;
        stopwatchMinutes = totalMin % 60;
        stopwatchHours = totalMin / 60;
    }
}

void updateTimer() {
    if (timerRunning) {
        int remaining = timerTotalSeconds - timerClock.getElapsedTime().asSeconds();
        if (remaining <= 0) {
            timerRunning = false;
            timerFinished = true;
            timerHours = timerMinutes = timerSeconds = 0;
            if (!timerSoundPlayed) {
                initializeAlarmSound();
                if (alarmSound.getStatus() != Sound::Playing) alarmSound.play();
                timerSoundPlayed = true;
            }
        } else {
            timerHours = remaining / 3600;
            timerMinutes = (remaining % 3600) / 60;
            timerSeconds = remaining % 60;
        }
    }
}

void drawText(RenderWindow& w, Font& f, const char* str, int size, Color c, float x, float y, bool center = false) {
    Text t;
    t.setFont(f);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(c);
    if (center) {
        FloatRect b = t.getLocalBounds();
        t.setPosition(x - b.width / 2, y);
    } else t.setPosition(x, y);
    w.draw(t);
}

void drawButton(RenderWindow& w, Font& f, float x, float y, float wd, float ht, const char* txt, bool sel) {
    RectangleShape btn(Vector2f(wd, ht));
    btn.setPosition(x, y);
    btn.setFillColor(sel ? Color(100, 150, 200) : Color(80, 80, 80));
    btn.setOutlineColor(Color::White);
    btn.setOutlineThickness(2);
    w.draw(btn);
    drawText(w, f, txt, 20, Color::White, x + wd/2, y + ht/2, true);
}

void drawInputBox(RenderWindow& w, Font& f, float x, float y, float wd, float ht, const char* lbl, int val) {
    RectangleShape box(Vector2f(wd, ht));
    box.setPosition(x, y);
    box.setFillColor(Color(40, 40, 40));
    box.setOutlineColor(Color::White);
    box.setOutlineThickness(2);
    w.draw(box);
    drawText(w, f, lbl, 18, Color::White, x + 10, y - 25);
    char valStr[10];
    sprintf(valStr, "%02d", val);
    drawText(w, f, valStr, 24, Color::Yellow, x + wd/2, y + ht/2, true);
}

void drawClock(RenderWindow& w, Font& f, int h, int m, int s) {
    char timeStr[20];
    formatTime(h, m, s, timeStr, is24Hour);
    drawText(w, f, timeStr, 80, alarmTriggered ? Color::Red : Color::White, WINDOW_WIDTH/2, 150, true);
    char fmt[20];
    sprintf(fmt, "Format: %s", is24Hour ? "24H" : "12H");
    drawButton(w, f, WINDOW_WIDTH/2 - 100, 280, 200, 40, fmt, false);
}

void drawAlarmMenu(RenderWindow& w, Font& f) {
    drawText(w, f, "ALARM SETTINGS", 40, Color::White, WINDOW_WIDTH/2, 30, true);
    
    float alarmListY = 100;
    float alarmListWidth = 600;
    float alarmListX = (WINDOW_WIDTH - alarmListWidth) / 2;
    
    for (int i = 0; i < alarmCount; i++) {
        char str[100];
        sprintf(str, "%02d:%02d - %s [%s]", alarmHours[i], alarmMinutes[i], 
                alarmDescriptions[i], alarmEnabled[i] ? "ON" : "OFF");
        drawText(w, f, str, 20, alarmEnabled[i] ? Color::Green : Color(128, 128, 128), 
                alarmListX, alarmListY + i * 40);
    }
    
    if (inputMode && inputType < 2) {
        const char* labels[] = {"Hour (0-23)", "Minute (0-59)"};
        float boxWidth = 100, boxSpacing = 20;
        float totalWidth = boxWidth * 2 + boxSpacing;
        float startX = (WINDOW_WIDTH - totalWidth) / 2;
        float xs[] = {startX, startX + boxWidth + boxSpacing};
        drawInputBox(w, f, xs[inputType], 400, boxWidth, 50, labels[inputType], inputValue);
    }
    
    float btnWidth = 150, btnHeight = 40, btnSpacing = 20;
    float btnTotalWidth = btnWidth * 4 + btnSpacing * 3;
    float btnStartX = (WINDOW_WIDTH - btnTotalWidth) / 2;
    
    drawButton(w, f, btnStartX, 500, btnWidth, btnHeight, "Add Alarm", inputMode && inputType == 0);
    drawButton(w, f, btnStartX + btnWidth + btnSpacing, 500, btnWidth, btnHeight, "Toggle Alarm", false);
    drawButton(w, f, btnStartX + (btnWidth + btnSpacing) * 2, 500, btnWidth, btnHeight, "Delete Alarm", false);
    drawButton(w, f, btnStartX + (btnWidth + btnSpacing) * 3, 500, btnWidth, btnHeight, "Back", false);
    
    drawText(w, f, "Press 1-9 to select alarm, A to add, T to toggle, D to delete", 16, Color::Yellow, WINDOW_WIDTH/2, 600, true);
}

void drawStopwatch(RenderWindow& w, Font& f) {
    drawText(w, f, "STOPWATCH", 40, Color::White, WINDOW_WIDTH/2, 30, true);
    
    char str[30];
    sprintf(str, "%02d:%02d:%02d.%03d", stopwatchHours, stopwatchMinutes, stopwatchSeconds, stopwatchMilliseconds / 10);
    drawText(w, f, str, 80, Color::Cyan, WINDOW_WIDTH/2, 200, true);
    
    const char* status = stopwatchRunning && !stopwatchPaused ? "RUNNING" : stopwatchPaused ? "PAUSED" : "STOPPED";
    drawText(w, f, status, 30, Color::Yellow, WINDOW_WIDTH/2, 320, true);
    
    drawButton(w, f, 150, 400, 150, 50, "Start", false);
    drawButton(w, f, 320, 400, 150, 50, "Stop", false);
    drawButton(w, f, 490, 400, 150, 50, "Reset", false);
    drawButton(w, f, 660, 400, 150, 50, "Back", false);
}

void drawTimer(RenderWindow& w, Font& f) {
    drawText(w, f, "COUNTDOWN TIMER", 40, Color::White, WINDOW_WIDTH/2, 30, true);
    
    char str[20];
    sprintf(str, "%02d:%02d:%02d", timerHours, timerMinutes, timerSeconds);
    drawText(w, f, str, 80, timerFinished ? Color::Red : Color::Magenta, WINDOW_WIDTH/2, 150, true);
    
    if (timerFinished) {
        drawText(w, f, "TIME'S UP!", 40, Color::Red, WINDOW_WIDTH/2, 250, true);
        drawText(w, f, "Press SPACE to dismiss", 20, Color::White, WINDOW_WIDTH/2, 310, true);
        drawButton(w, f, WINDOW_WIDTH/2 - 75, 380, 150, 50, "Reset", false);
        drawButton(w, f, WINDOW_WIDTH/2 - 75, 450, 150, 50, "Back", false);
    } else {
        float boxWidth = 100, boxHeight = 50, boxSpacing = 20;
        float totalWidth = boxWidth * 3 + boxSpacing * 2;
        float startX = (WINDOW_WIDTH - totalWidth) / 2;
        
        if (inputMode && inputType >= 2) {
            const char* labels[] = {"Hours", "Minutes", "Seconds"};
            float xs[] = {startX, startX + boxWidth + boxSpacing, startX + (boxWidth + boxSpacing) * 2};
            drawInputBox(w, f, xs[inputType - 2], 350, boxWidth, boxHeight, labels[inputType - 2], inputValue);
        } else {
            drawInputBox(w, f, startX, 350, boxWidth, boxHeight, "Hours", timerHours);
            drawInputBox(w, f, startX + boxWidth + boxSpacing, 350, boxWidth, boxHeight, "Minutes", timerMinutes);
            drawInputBox(w, f, startX + (boxWidth + boxSpacing) * 2, 350, boxWidth, boxHeight, "Seconds", timerSeconds);
        }
        
        float btnWidth = 150, btnHeight = 50, btnSpacing = 20;
        float btnTotalWidth = btnWidth * 4 + btnSpacing * 3;
        float btnStartX = (WINDOW_WIDTH - btnTotalWidth) / 2;
        
        drawButton(w, f, btnStartX, 450, btnWidth, btnHeight, "Set Time", false);
        drawButton(w, f, btnStartX + btnWidth + btnSpacing, 450, btnWidth, btnHeight, "Start", false);
        drawButton(w, f, btnStartX + (btnWidth + btnSpacing) * 2, 450, btnWidth, btnHeight, "Stop", false);
        drawButton(w, f, btnStartX + (btnWidth + btnSpacing) * 3, 450, btnWidth, btnHeight, "Back", false);
    }
}

void drawCalendar(RenderWindow& w, Font& f, int d, int m, int y) {
    drawText(w, f, "CALENDAR", 40, Color::White, WINDOW_WIDTH/2, 30, true);
    
    char dateStr[50];
    formatDate(d, m, y, dateStr);
    drawText(w, f, dateStr, 50, Color::White, WINDOW_WIDTH/2, 100, true);
    
    struct tm t;
    memset(&t, 0, sizeof(struct tm));
    t.tm_mday = d;
    t.tm_mon = m - 1;
    t.tm_year = y - 1900;
    t.tm_hour = 12;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = -1;
    time_t dateTime = mktime(&t);
    if (dateTime == -1) {
        time_t now = time(0);
        struct tm* nowInfo = localtime(&now);
        dateTime = mktime(nowInfo);
    }
    struct tm* dateInfo = localtime(&dateTime);
    
    const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    drawText(w, f, days[dateInfo->tm_wday], 40, Color::Cyan, WINDOW_WIDTH/2, 170, true);
    
    const char* dayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    float cellWidth = 80, cellHeight = 30;
    float gridWidth = cellWidth * 7;
    float startX = (WINDOW_WIDTH - gridWidth) / 2;
    float startY = 240;
    
    for (int i = 0; i < 7; i++)
        drawText(w, f, dayNames[i], 18, Color::Yellow, startX + i * cellWidth + cellWidth/2, startY, true);
    
    struct tm firstDay;
    memset(&firstDay, 0, sizeof(struct tm));
    firstDay.tm_mday = 1;
    firstDay.tm_mon = m - 1;
    firstDay.tm_year = y - 1900;
    firstDay.tm_hour = 12;
    firstDay.tm_min = 0;
    firstDay.tm_sec = 0;
    firstDay.tm_isdst = -1;
    time_t firstDayTime = mktime(&firstDay);
    if (firstDayTime == -1) {
        time_t now = time(0);
        struct tm* nowInfo = localtime(&now);
        firstDayTime = mktime(nowInfo);
    }
    int firstWeekday = localtime(&firstDayTime)->tm_wday;
    
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))) daysInMonth[1] = 29;
    int daysInCurr = daysInMonth[m - 1];
    
    int currDay = 1;
    for (int week = 0; week < 6 && currDay <= daysInCurr; week++) {
        for (int weekday = 0; weekday < 7 && currDay <= daysInCurr; weekday++) {
            if (week == 0 && weekday < firstWeekday) continue;
            char dayNum[5];
            sprintf(dayNum, "%d", currDay);
            drawText(w, f, dayNum, 20, currDay == d ? Color::Red : Color::White, 
                    startX + weekday * cellWidth + cellWidth/2, startY + 35 + week * cellHeight, true);
            currDay++;
        }
    }
    
    drawButton(w, f, WINDOW_WIDTH/2 - 75, 600, 150, 40, "Back", false);
}

void drawMenu(RenderWindow& w, Font& f) {
    drawText(w, f, "DIGITAL CLOCK APPLICATION", 35, Color::White, WINDOW_WIDTH/2, 50, true);
    const char* modes[] = {"Clock", "Alarm", "Stopwatch", "Timer", "Calendar", "Exit"};
    for (int i = 0; i < 6; i++)
        drawButton(w, f, WINDOW_WIDTH/2 - 100, 150 + i * 70, 200, 50, modes[i], currentMode == i);
}

int main() {
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Digital Clock Application");
    window.setFramerateLimit(60);
    
    Font font;
    const char* fontPaths[] = {
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
        "Data/arial.ttf"
    };
    bool fontLoaded = false;
    for (int i = 0; i < 6; i++) {
        if (font.loadFromFile(fontPaths[i])) { fontLoaded = true; break; }
    }
    if (!fontLoaded) cerr << "Warning: Could not load font." << endl;
    
    initializeAlarms();
    loadAlarms();
    initializeAlarmSound();
    
    Clock secondClock;
    int h, m, s, d, mo, y;
    
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) { saveAlarms(); window.close(); }
            
            if (event.type == Event::KeyPressed) {
                if (alarmTriggered && (event.key.code == Keyboard::Space || event.key.code == Keyboard::Return || event.key.code == Keyboard::Enter)) {
                    alarmTriggered = false;
                    alarmBlinkCounter = 0;
                    alarmSound.stop();
                    continue;
                }
                
                if (!inputMode) {
                    int key = event.key.code;
                    if (key == Keyboard::Num1 || key == Keyboard::Numpad1) currentMode = 0;
                    else if (key == Keyboard::Num2 || key == Keyboard::Numpad2) currentMode = 1;
                    else if (key == Keyboard::Num3 || key == Keyboard::Numpad3) currentMode = 2;
                    else if (key == Keyboard::Num4 || key == Keyboard::Numpad4) currentMode = 3;
                    else if (key == Keyboard::Num5 || key == Keyboard::Numpad5) currentMode = 4;
                    else if (key == Keyboard::Escape || key == Keyboard::Num6) {
                        if (currentMode == 0) window.close();
                        else currentMode = 0;
                    }
                }
                
                if (currentMode == 0 && event.key.code == Keyboard::F) is24Hour = !is24Hour;
                
                if (currentMode == 1) {
                    int key = event.key.code;
                    if (key == Keyboard::A && alarmCount < MAX_ALARMS) {
                        inputMode = true; inputType = 0; inputValue = 0; currentAlarmIndex = alarmCount;
                    } else if (key == Keyboard::T && alarmCount > 0) {
                        int idx = (currentAlarmIndex < alarmCount) ? currentAlarmIndex : alarmCount - 1;
                        alarmEnabled[idx] = !alarmEnabled[idx];
                        saveAlarms();
                    } else if (key == Keyboard::D && alarmCount > 0) {
                        int idx = (currentAlarmIndex < alarmCount) ? currentAlarmIndex : alarmCount - 1;
                        for (int i = idx; i < alarmCount - 1; i++) {
                            alarmHours[i] = alarmHours[i + 1];
                            alarmMinutes[i] = alarmMinutes[i + 1];
                            alarmEnabled[i] = alarmEnabled[i + 1];
                            strcpy(alarmDescriptions[i], alarmDescriptions[i + 1]);
                        }
                        alarmCount--;
                        saveAlarms();
                    } else if (key >= Keyboard::Num1 && key <= Keyboard::Num9) {
                        int num = key - Keyboard::Num1;
                        if (num < alarmCount) currentAlarmIndex = num;
                    }
                    
                    if (inputMode) {
                        if (event.key.code == Keyboard::Enter) {
                            if (inputType == 0 && inputValue >= 0 && inputValue < 24) {
                                alarmHours[currentAlarmIndex] = inputValue;
                                inputType = 1; inputValue = 0;
                            } else if (inputType == 1 && inputValue >= 0 && inputValue < 60) {
                                alarmMinutes[currentAlarmIndex] = inputValue;
                                alarmEnabled[currentAlarmIndex] = true;
                                strcpy(alarmDescriptions[currentAlarmIndex], "Alarm");
                                if (currentAlarmIndex == alarmCount) alarmCount++;
                                inputMode = false;
                                saveAlarms();
                            }
                        } else if (event.key.code == Keyboard::Escape) inputMode = false;
                        else if (key >= Keyboard::Num0 && key <= Keyboard::Num9) {
                            int digit = key - Keyboard::Num0;
                            inputValue = inputValue * 10 + digit;
                            if (inputType == 0 && inputValue > 23) inputValue = 23;
                            if (inputType == 1 && inputValue > 59) inputValue = 59;
                        } else if (key == Keyboard::BackSpace) inputValue = inputValue / 10;
                    }
                }
                
                if (currentMode == 2) {
                    int key = event.key.code;
                    if (key == Keyboard::S && !stopwatchRunning) {
                        stopwatchRunning = true; stopwatchPaused = false; stopwatchClock.restart();
                    } else if (key == Keyboard::P && stopwatchRunning) {
                        stopwatchPaused = !stopwatchPaused;
                        if (!stopwatchPaused) stopwatchClock.restart();
                    } else if (key == Keyboard::R) {
                        stopwatchRunning = stopwatchPaused = false;
                        stopwatchHours = stopwatchMinutes = stopwatchSeconds = stopwatchMilliseconds = 0;
                    }
                }
                
                if (currentMode == 3) {
                    int key = event.key.code;
                    if (key == Keyboard::S && !inputMode && timerTotalSeconds > 0) {
                        timerRunning = true; timerFinished = false; timerSoundPlayed = false; timerClock.restart();
                    } else if (key == Keyboard::P) {
                        timerRunning = false;
                        if (timerFinished) { alarmSound.stop(); timerSoundPlayed = false; }
                    } else if (key == Keyboard::T) {
                        inputMode = true; inputType = 2; inputValue = 0;
                        timerRunning = timerFinished = false;
                        alarmSound.stop(); timerSoundPlayed = false;
                    } else if (key == Keyboard::R) {
                        timerRunning = timerFinished = false;
                        timerHours = timerMinutes = timerSeconds = timerTotalSeconds = 0;
                        alarmSound.stop(); timerSoundPlayed = false;
                    } else if (timerFinished && (key == Keyboard::Space || key == Keyboard::Return || key == Keyboard::Enter)) {
                        timerFinished = false;
                        alarmSound.stop();
                        timerSoundPlayed = false;
                    }
                    
                    if (inputMode) {
                        if (event.key.code == Keyboard::Enter) {
                            if (inputType == 2) { timerHours = inputValue; inputType = 3; inputValue = 0; }
                            else if (inputType == 3) { timerMinutes = inputValue; inputType = 4; inputValue = 0; }
                            else if (inputType == 4) {
                                timerSeconds = inputValue;
                                timerTotalSeconds = timerHours * 3600 + timerMinutes * 60 + timerSeconds;
                                inputMode = false;
                            }
                        } else if (event.key.code == Keyboard::Escape) inputMode = false;
                        else if (key >= Keyboard::Num0 && key <= Keyboard::Num9) {
                            int digit = key - Keyboard::Num0;
                            inputValue = inputValue * 10 + digit;
                            if (inputType == 2 && inputValue > 99) inputValue = 99;
                            if ((inputType == 3 || inputType == 4) && inputValue > 59) inputValue = 59;
                        } else if (key == Keyboard::BackSpace) inputValue = inputValue / 10;
                    }
                }
            }
        }
        
        if (secondClock.getElapsedTime().asSeconds() >= 1.0f) {
            updateClock(h, m, s, d, mo, y);
            checkAlarms(h, m);
            secondClock.restart();
        }
        
        if (currentMode == 2) updateStopwatch();
        if (currentMode == 3) updateTimer();
        
        if (alarmTriggered) {
            alarmBlinkCounter++;
            if (alarmBlinkCounter > 30) alarmBlinkCounter = 0;
        }
        
        window.clear(Color(20, 20, 30));
        
        if (currentMode == 0) {
            drawClock(window, font, h, m, s);
            if (alarmTriggered) {
                drawText(window, font, "ALARM!", 50, (alarmBlinkCounter / 5) % 2 == 0 ? Color::Red : Color::Yellow, WINDOW_WIDTH/2, 350, true);
                drawText(window, font, "Press SPACE to dismiss", 20, Color::White, WINDOW_WIDTH/2, 420, true);
            }
        } else if (currentMode == 1) drawAlarmMenu(window, font);
        else if (currentMode == 2) drawStopwatch(window, font);
        else if (currentMode == 3) drawTimer(window, font);
        else if (currentMode == 4) drawCalendar(window, font, d, mo, y);
        else drawMenu(window, font);
        
        window.display();
    }
    
    saveAlarms();
    return 0;
}
