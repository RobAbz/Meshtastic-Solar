/**
 * Time Management Module Implementation
 * Handles RTC, timekeeping, and timestamp formatting
 */

#include "timekeeper.h"
#include <Preferences.h>

// Static member initialization
uint32_t TimeKeeper::startMillis = 0;
uint32_t TimeKeeper::currentTimestamp = 0;
bool TimeKeeper::timeSet = false;

// Preferences object
static Preferences timePrefs;

void TimeKeeper::init() {
    startMillis = millis();
    
    // Try to load saved time
    load();
    
    if (!timeSet) {
        // Set default time to compile time
        // This is a rough approximation
        currentTimestamp = 0;  // Epoch
        DEBUG_PRINTLN("Time not set, using default");
    }
    
    DEBUG_PRINTLN("TimeKeeper initialized");
}

void TimeKeeper::setDateTime(uint16_t year, uint8_t month, uint8_t day,
                            uint8_t hour, uint8_t minute, uint8_t second) {
    // Convert to Unix timestamp (seconds since Jan 1, 1970)
    // Simplified calculation
    
    // Days in each month (non-leap year)
    const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Calculate days since epoch
    uint32_t days = 0;
    
    // Add years
    for (uint16_t y = 1970; y < year; y++) {
        bool isLeap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        days += isLeap ? 366 : 365;
    }
    
    // Add months
    for (uint8_t m = 1; m < month; m++) {
        days += daysInMonth[m - 1];
        // Add leap day if February and leap year
        if (m == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if (isLeap) days++;
        }
    }
    
    // Add days
    days += day - 1;
    
    // Convert to seconds and add time
    currentTimestamp = days * 86400UL + hour * 3600UL + minute * 60UL + second;
    
    // Reset start millis
    startMillis = millis();
    timeSet = true;
    
    DEBUG_PRINTF("Time set to: %04d-%02d-%02d %02d:%02d:%02d\n",
               year, month, day, hour, minute, second);
    
    // Save to preferences
    save();
}

uint32_t TimeKeeper::getTimestamp() {
    if (timeSet) {
        // Calculate elapsed seconds since last set
        uint32_t elapsedMs = millis() - startMillis;
        uint32_t elapsedSeconds = elapsedMs / 1000;
        return currentTimestamp + elapsedSeconds;
    }
    return 0;
}

void TimeKeeper::getDateTimeString(char* buffer) {
    if (!timeSet) {
        strcpy(buffer, "Time Not Set");
        return;
    }
    
    uint32_t ts = getTimestamp();
    
    // Convert timestamp to date/time components
    uint32_t seconds = ts % 60;
    uint32_t minutes = (ts / 60) % 60;
    uint32_t hours = (ts / 3600) % 24;
    uint32_t days = ts / 86400;
    
    // Calculate year, month, day from days since epoch
    uint16_t year = 1970;
    uint8_t month = 1;
    uint8_t day = 1;
    
    // Simplified date calculation
    const uint16_t daysInYear = 365;
    while (days >= daysInYear) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        uint16_t yearDays = isLeap ? 366 : 365;
        if (days >= yearDays) {
            days -= yearDays;
            year++;
        } else {
            break;
        }
    }
    
    // Month and day calculation
    const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    
    for (month = 1; month <= 12; month++) {
        uint8_t monthDays = daysInMonth[month - 1];
        if (month == 2 && isLeap) monthDays = 29;
        
        if (days < monthDays) {
            day = days + 1;
            break;
        }
        days -= monthDays;
    }
    
    snprintf(buffer, 20, "%04d-%02d-%02d %02d:%02d:%02d",
           year, month, day, (int)hours, (int)minutes, (int)seconds);
}

void TimeKeeper::getDateString(char* buffer) {
    char dateTime[20];
    getDateTimeString(dateTime);
    
    // Extract date part (first 10 characters)
    strncpy(buffer, dateTime, 10);
    buffer[10] = '\0';
}

void TimeKeeper::getTimeString(char* buffer) {
    char dateTime[20];
    getDateTimeString(dateTime);
    
    // Extract time part (last 8 characters)
    if (strlen(dateTime) >= 19) {
        strcpy(buffer, dateTime + 11);
    } else {
        strcpy(buffer, "00:00:00");
    }
}

void TimeKeeper::update() {
    // Called periodically to update time
    // Nothing to do here since getTimestamp() calculates dynamically
}

void TimeKeeper::save() {
    timePrefs.begin(PREFS_NAMESPACE, false);
    timePrefs.putUInt("timestamp", currentTimestamp);
    timePrefs.putUInt("start_ms", startMillis);
    timePrefs.putBool("time_set", timeSet);
    timePrefs.end();
    
    DEBUG_PRINTLN("Time saved to preferences");
}

void TimeKeeper::load() {
    timePrefs.begin(PREFS_NAMESPACE, true);  // read-only
    
    timeSet = timePrefs.getBool("time_set", false);
    if (timeSet) {
        currentTimestamp = timePrefs.getUInt("timestamp", 0);
        uint32_t savedStartMs = timePrefs.getUInt("start_ms", 0);
        
        // Calculate time elapsed since save
        uint32_t currentMs = millis();
        uint32_t elapsedMs;
        
        if (currentMs >= savedStartMs) {
            elapsedMs = currentMs - savedStartMs;
        } else {
            // Millis wrapped around
            elapsedMs = (0xFFFFFFFF - savedStartMs) + currentMs;
        }
        
        // Adjust timestamp
        currentTimestamp += elapsedMs / 1000;
        startMillis = millis();
        
        DEBUG_PRINTLN("Time loaded from preferences");
    }
    
    timePrefs.end();
}
