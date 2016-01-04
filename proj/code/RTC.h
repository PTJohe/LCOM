#ifndef __RTC_H
#define __RTC_H

static int hook_id_rtc;

/** @defgroup RTC RTC
 * @{
 * Functions to manipulate the real-time clock
 */

/**
 * @brief Subscribes RTC interrupts
 * @return 0 on success, non-zero otherwise
 */
int subscribeRTC();
/**
 * @brief Unsubscribes RTC interrupts
 * @return 0 on success, non-zero otherwise
 */
int unsubscribeRTC();

/**
 * @brief Reads RTC register data
 *
 * @param array Address location
 * @return Requested value
 */
short readRTC(long array);
/**
 * @brief Returns the current date and time
 *
 * @return String with date and time
 */
char* getDateRTC();

/** @} end of RTC */
#endif /* __RTC_H */
