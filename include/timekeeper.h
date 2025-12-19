#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#include <Arduino.h>
#include "config.h"

/**
 * Time Management Module
 * Handles RTC, timekeeping, and timestamp formatting
 */

class TimeKeeper {
public:
    /**
     * Initialize time system
     */
    static void init();
    
    /**
     * Set current date and time
     * @param year 2000-2099
     * @param month 1-12
     * @param day 1-31
     * @param hour 0-23
     * @param minute 0-59
     * @param second 0-59
     */
    static void setDateTime(uint16_t year, uint8_t month, uint8_t day,
                           uint8_t hour, uint8_t minute, uint8_t second);
    
    /**
     * Get current timestamp in seconds since epoch
     */
    static uint32_t getTimestamp();
    
    /**
     * Get formatted date/time string
     * @param buffer Buffer to write to (min 20 bytes)
     * Format: "YYYY-MM-DD HH:MM:SS"
     */
    static void getDateTimeString(char* buffer);
    
    /**
     * Get formatted date string
     * @param buffer Buffer to write to (min 11 bytes)
     * Format: "YYYY-MM-DD"
     */
    static void getDateString(char* buffer);
    
    /**
     * Get formatted time string
     * @param buffer Buffer to write to (min 9 bytes)
     * Format: "HH:MM:SS"
     */
    static void getTimeString(char* buffer);
    
    /**
     * Update time (call periodically)
     */
    static void update();
    
    /**
     * Save time to preferences
     */
    static void save();
    
    /**
     * Load time from preferences
     */
    static void load();

private:
    static uint32_t startMillis;
    static uint32_t currentTimestamp;
    static bool timeSet;
};

#endif // TIMEKEEPER_H
