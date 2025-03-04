/* The true ESP32 chip ID is essentially its MAC address.
This sketch provides an alternate chip ID that matches 
the output of the ESP.getChipId() function on ESP8266 
(i.e. a 32-bit integer matching the last 3 bytes of 
the MAC address. This is less unique than the 
MAC address chip ID, but is helpful when you need 
an identifier that can be no more than a 32-bit integer 
(like for switch...case).

created 2020-06-07 by cweinhofer
with help from Cicicok */

// BOARD => ESP32S3 Dev Module
// PORT  => COM6 (may differ on different machines)

//
// Taken from: https://www.teachmemicro.com/using-the-1-77-st7735-tft-lcd-with-esp32/
// And: https://www.instructables.com/Value-Your-Project-Use-Graphic-Display/
// Ethernet: https://github.com/PuceBaboon/ESP32_W5500_NTP_CLIENT/blob/master/src/ESP32_NTP.ino
// DHCP: https://docs.arduino.cc/tutorials/ethernet-shield-rev2/dhcp-address-printer/
//
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Adafruit_ST7735.h>

#include "local_config.h"   // Has ETH configuration

// LCD Display Constants
#define TFT_CS 21
#define TFT_RST 15
#define TFT_DC 16
#define TFT_MOSI 17 // SDA
#define TFT_CLK 18 // SCK

#define TFT_TEXT_SIZE 1
#define TFT_LINE_HEIGHT 10
#define TFT_LINE_LENGTH 21    // CHARS
#define TFT_NUMBER_LINES 16   // 0-15 lines
#define TFT_BGD_COLOR ST77XX_BLUE
#define TFT_TXT_COLOR ST77XX_WHITE

// Create an instance of the ST7735 class
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

uint32_t chipId = 0;

const int NTP_PACKET_SIZE = 48;		// NTP time stamp is in the first 48 bytes of the message.
byte packetBuffer[NTP_PACKET_SIZE];	// Buffer for both incoming and outgoing packets.

// A UDP instance to let us send and receive packets over UDP.
EthernetUDP Udp;

char utc_time_string[9];

// Writes a line of text to the display.
void write_text_to_lcd(String text, int line_number) {
  // Clear the line first 
  tft.fillRect(0, line_number * TFT_LINE_HEIGHT, 128, TFT_LINE_HEIGHT, TFT_BGD_COLOR);

  // Now display the text
  tft.setCursor(0, line_number * TFT_LINE_HEIGHT);
  tft.setTextColor(TFT_TXT_COLOR);
  tft.println(text);
}

void prt_hwval(uint8_t refval) {
    switch (refval) {
    case 0:
        Serial.println("No hardware detected.");
        break;
    case 1:
        Serial.println("WizNet W5100 detected.");
        break;
    case 2:
        Serial.println("WizNet W5200 detected.");
        break;
    case 3:
        Serial.println("WizNet W5500 detected.");
        break;
    default:
        Serial.println
            ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
    }
}

void prt_ethval(uint8_t refval) {
    switch (refval) {
    case 0:
        Serial.println("Uknown status.");
        break;
    case 1:
        Serial.println("Link flagged as UP.");
        break;
    case 2:
        Serial.println("Link flagged as DOWN. Check cable connection.");
        break;
    default:
        Serial.println
            ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
    }
}

// Send an NTP request to the time server at the given address (defined in local_conf.h).
void sendNTPpacket(const char *address) {
    // Set all bytes in the buffer to 0.
    memset(packetBuffer, 0, NTP_PACKET_SIZE);

    // Initialize values needed to form NTP request
    // (see http://en.wikipedia.org/wiki/Network_Time_Protocol).
    packetBuffer[0] = 0b11100011;      // LI, Version, Mode
    packetBuffer[1] = 0;        // Stratum, or type of clock
    packetBuffer[2] = 6;        // Polling Interval
    packetBuffer[3] = 0xEC;     // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // All NTP fields have been given values, now
    // send a packet requesting a timestamp.
    Udp.beginPacket(address, 123);      // NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}

void setup() {
	Serial.begin(115200);

  // Initialize the display
  tft.initR(INITR_BLACKTAB); // For 1.8" displays, use INITR_144GREENTAB for 1.44"
  tft.setRotation(0); // Adjust orientation

  // Clear the screen with a color
  tft.fillScreen(TFT_BGD_COLOR);  

  // Display text
  tft.setTextColor(TFT_TXT_COLOR);
  tft.setTextSize(TFT_TEXT_SIZE);

  write_text_to_lcd("012345678901234567890", 0);  // 21 chars wide at this text size
  write_text_to_lcd("1", 1);
  write_text_to_lcd("2", 2);
  write_text_to_lcd("3", 3);
  write_text_to_lcd("4", 4);

  write_text_to_lcd("5", 5);
  write_text_to_lcd("5 XXXX", 5);

  write_text_to_lcd("6", 6);
  write_text_to_lcd("7", 7);
  write_text_to_lcd("8", 8);  
  write_text_to_lcd("9", 9);    
  write_text_to_lcd("10", 10);  
  write_text_to_lcd("11", 11);
  write_text_to_lcd("12", 12);  
  write_text_to_lcd("13", 13);  
  write_text_to_lcd("14", 14);
  write_text_to_lcd("15", 15);

  Ethernet.init(10);           // GPIO10 on the ESP32.
  Serial.println("Starting ETHERNET connection...");
  // Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);
  Ethernet.begin(eth_MAC);
  delay(1000);

  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP()); 

  write_text_to_lcd(Ethernet.localIP().toString(), 5);

  /*
   * Sanity checks for W5500 and cable connection.
  */
  Serial.print("Checking connection.");
  bool rdy_flag = false;
    for (uint8_t i = 0; i <= 20; i++) {
      if ((Ethernet.hardwareStatus() == EthernetNoHardware)
            || (Ethernet.linkStatus() == LinkOFF)) {
            Serial.print(".");
            rdy_flag = false;
            delay(80);
        } else {
            rdy_flag = true;
            break;
        }
    }
    if (rdy_flag == false) {
        Serial.println
            ("\n\r\tHardware fault, or cable problem... cannot continue.");
        Serial.print("Hardware Status: ");
        prt_hwval(Ethernet.hardwareStatus());
        Serial.print("   Cable Status: ");
        prt_ethval(Ethernet.linkStatus());
        while (true) {
            delay(10);          // Halt.
        }
    } else {
        Serial.println(" OK");
    }

    Udp.begin(localPort);  
}

void loop() {

  sendNTPpacket(timeServer);  // Send an NTP packet to the time server.

	delay(1000);

    if (Udp.parsePacket()) {
        // We've received a packet, read the data from it.
        Udp.read(packetBuffer, NTP_PACKET_SIZE);        // Read the packet into the buffer.

        // The timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, extract the two words.
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

        // Next, combine the four bytes (two words) into a long integer.
        // This is NTP time (seconds since Jan 1 1900).
        unsigned long secsSince1900 = highWord << 16 | lowWord;

        // Now convert NTP time into everyday time.
        Serial.print("Unix time = ");

        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800.
        const unsigned long seventyYears = 2208988800UL;

        // Subtract seventy years.
        unsigned long epoch = secsSince1900 - seventyYears;

        // ...and then print Unix time.
        Serial.print(epoch);

        // Print the hour, minute and second.
        sprintf(utc_time_string, "%02d:%02d:%02d", (epoch % 86400L) / 3600, (epoch % 3600) / 60, epoch % 60);
        write_text_to_lcd(utc_time_string, 15);

        Serial.print(", The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT).         
        Serial.println(utc_time_string);
    }  
}