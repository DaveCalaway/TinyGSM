/*
   smsRead

   The software, on Arduino Mini Pro, wait the sms from the SIM900.

   Davide Gariselli - Mobimentum 2019
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

// Select your modem:
#define TINY_GSM_MODEM_SIM900
//#define DUMP_AT_COMMANDS

// set GSM PIN, if any
#define GSM_PIN "3577"

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon Serial

// Software Serial
SoftwareSerial SerialAT(2, 3); // RX, TX

#define TINY_GSM_DEBUG SerialMon

// Module baud rate
uint32_t rate = 0;  // Set to 0 for Auto-Detect

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

String SMS = "";
String ID = "";

// ============== MODEM ==============
void setUpModem() {
    delay(3000);  // sanity delay

    if (!rate) {
        rate = TinyGsmAutoBaud(SerialAT);
    }
    SerialAT.begin(rate);

    String modemInfo = modem.getModemInfo();
    DBG("Modem:", modemInfo);

    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3) {
        modem.simUnlock(GSM_PIN);
    }

    DBG("Waiting for network...");
    if (!modem.waitForNetwork()) {
        delay(10000);
        return;
    }

    if (modem.isNetworkConnected()) {
        DBG("Network connected");
    }

    String ccid = modem.getSimCCID();
    DBG("CCID:", ccid);

    String imei = modem.getIMEI();
    DBG("IMEI:", imei);

    String cop = modem.getOperator();
    DBG("Operator:", cop);

    IPAddress local = modem.localIP();
    DBG("Local IP:", local);

    int csq = modem.getSignalQuality();
    DBG("Signal quality:", csq);

    // This is only supported on SIMxxx series
    //String gsmLoc = modem.getGsmLocation(); // -------- > 601 ERROR
    //DBG("GSM location:", gsmLoc);

    // This is only supported on SIMxxx series
    String gsmTime = modem.getGSMDateTime(DATE_TIME);
    DBG("GSM Time:", gsmTime);
    String gsmDate = modem.getGSMDateTime(DATE_DATE);
    DBG("GSM Date:", gsmDate);

    delay(1000);
}

// ============== SETUP ==============
void setup() {
    // Set console baud rate
    SerialMon.begin(115200);
    setUpModem();
}

// ============== LOOP ==============
void loop() {
    DBG("SMS test read");
    /*
           set mode 0 for reading latest unread message first
           set mode 1 for reading oldest unread message first
           in newMessageIndex(mode);
    */
    int index = modem.newMessageIndex(0);

    if (index > 0) {
        SMS = modem.readSMS(index);
        ID = modem.getSenderID(index);
        DBG("new message arrived from :");
        DBG(ID);
        DBG("Says");
        DBG(SMS);
    }
    delay(2000);
}
