/*
 *   ESP32 Template
 *   Optional NTP client Settings
 */
#ifndef NTP_CONFIG_H
#define NTP_CONFIG_H

#ifdef NTP_CLT
#include "time.h"
#include "esp_sntp.h"

//
// NTP Client configuration
//
// Timezone including DST rules (see: https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html)
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

// NTP update interval in ms
#define SYNC_INTERVAL 1800000

// Desired sync mode (SNTP_SYNC_MODE_IMMED or SNTP_SYNC_MODE_SMOOTH)
#define SYNC_MODE SNTP_SYNC_MODE_IMMED

// NTP server(s) to sync with (DNS name or IP)
#define NTPSRV_1 "at.pool.ntp.org"
//#define NTPSRV_2 "de.pool.ntp.org"

// Constructor for NTP sync callback function
extern void NTP_Synced_Callback(struct timeval *t);

// Global var increased on each time sync (set in NTP_Synced_Callback)
extern unsigned int NTPSyncCounter;
// Global var containing time info
extern struct tm TimeInfo;
// Configuration vars
extern const char* NTPServer1;
#ifdef NTPSRV_2
extern const char* NTPServer2;
#endif
extern const char* time_zone;


#endif //NTP_CLT
#endif // NTP_CONFIG_H