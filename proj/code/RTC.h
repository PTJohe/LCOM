#ifndef __RTC_H
#define __RTC_H

static int hook_id_rtc;

int subscribeRTC();
int unsubscribeRTC();

char* getDateRTC();

#endif
