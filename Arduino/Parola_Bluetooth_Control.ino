

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <EEPROM.h>
#include "Font_Data.h"

// BT interface serial interface
#define USE_ALTSOFTSERIAL 0

#if USE_ALTSOFTSERIAL
#include <AltSoftSerial.h>
#else
#include <SoftwareSerial.h>
#endif

// MAX72xx Definitions ----------------
// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define NUM_ZONES 2
#define ZONE_SIZE 4
#define MAX_DEVICES (NUM_ZONES * ZONE_SIZE)
#define CLK_PIN   12
#define DATA_PIN  10
#define CS_PIN    11

#define ZONE_UPPER  1
#define ZONE_LOWER  0
#define ZONE_ALL 2


#define PAUSE_TIME 0
#define SCROLL_SPEED 50

// Bluetooth Serial interface ---------
// Bluetooth Serial comms pins and parameters
const uint8_t BT_RECV_PIN = 2;   // Arduino receive -> Bluetooth TxD pin
const uint8_t BT_SEND_PIN = 3;   // Arduino send -> Bluetooth RxD pin
const char BT_NAME[] = "LEDotron";

// Define the type of hardware being used.
// Only one of these options is enabled at any time.
// The HM-10 is the JHHuaMao (HMSoft) version, not Bolutek. Line ending for AT commands differ.
#define HW_USE_HC05  0
#define HW_USE_HC06  1
#define HW_USE_HM10_HMSOFT 0
#define HW_USE_HM10_OTHER	 0

#if HW_USE_HC05
const uint8_t HC05_SETUP_ENABLE = 7;
#endif

// Miscellaneous
#define BUF_SIZE  100     // message buffer size
#define EEPROM_START_ADDR 0  // EEPROM address for setup info

//=====================================================
//======= END OF USER CONFIGURATION PARAMETERS ========
//=====================================================

// Turn on debug statements to the serial output
#define DEBUG 1

#if DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

#define SCROLL_LEFT 1

// Hardware adaptation parameters for scrolling
bool invertUpperZone = false;
textEffect_t scrollUpper, scrollLower;



// Serial protocol parameters
const uint16_t BT_COMMS_TIMEOUT = 1000; // Protocol packet timeout period (start to end packet within this period)

const char PKT_START = '*';    // protocol packet start character
const char PKT_END   = '~';    // protocol packet end character
const char PKT_ESC   = '#';    // protocol packet escape character

const char PKT_CMD_SPEED   = 'S'; // number (ms delay between frames)
const char PKT_CMD_BRIGHT  = 'B'; // Toggle 0-15
const char PKT_CMD_RESET   = 'R'; // Reset the Arduino hardware
const char PKT_CMD_FACSET  = 'F'; // Factory settings
const char PKT_CMD_SAVE    = 'W'; // Write current setup to EEPROM

const char PKT_CMD_MESSAGE = 'M'; // Displayed message
const char PKT_CMD_BMESSAGE = 'N'; // Displayed Bottom Message
const char PKT_CMD_HEIGHT = 'H'; // Toggle Height normal - double






const char PKT_CMD_JUSTIFY = 'J'; // Toggle L, C, R
const char PKT_CMD_INVERT  = 'V'; // Toggle Invert/Normal
const char PKT_CMD_TPAUSE  = 'P'; // number (ms delay between in and out)
const char PKT_CMD_IANIM   = 'I'; // In Animation - toggle through Animations table
const char PKT_CMD_OANIM   = 'O'; // Out animation - toggle through Animations table
const char PKT_CMD_SCROLL = 'T'; // Toggle SCROLLING - ZONETEXT


const char PKT_CMD_ACK  = 'Z';  // Acknowledge command - data is PKT_ERR_* defines
const char PKT_ERR_OK   = '0';  // no error/ok
const char PKT_ERR_TOUT = '1';  // timeout - start detected with no end within timeout period
const char PKT_ERR_CMD  = '2';  // command field not valid or unknown
const char PKT_ERR_DATA = '3';  // data field not valid
const char PKT_ERR_SEQ  = '4';  // generic protocol sequence error


const char *szStart = "AT+";
#if HW_USE_HC05
const char *szEnd   = "\r\n";
const char PROGMEM ATCmd[] = {"NAME=\0ROLE=0\0CLASS=800500\0RESET\0\0"};
#endif
#if HW_USE_HC06
const char *szEnd   = "\r\n";
const char PROGMEM ATCmd[] = {"NAME\0\0"};
#endif
#if HW_USE_HM10_HMSOFT
const char *szEnd   = "";
const char PROGMEM ATCmd[] = {"NAME\0TYPE0\0ROLE0\0RESET\0\0"};
#endif
#if HW_USE_HM10_OTHER
const char *szEnd   = "\r\n";
const char PROGMEM ATCmd[] = {"NAME\0TYPE0\0ROLE0\0RESET\0\0"};
#endif

// Data tables --------------
const textPosition_t textPosition[] = { PA_LEFT, PA_CENTER, PA_RIGHT };
const textEffect_t textEffect[] = 
{
  PA_PRINT, PA_SCROLL_UP, PA_SCROLL_LEFT, PA_SCROLL_RIGHT, PA_SCROLL_DOWN,
#if ENA_SCR_DIA
  PA_SCROLL_UP_LEFT, PA_SCROLL_UP_RIGHT, PA_SCROLL_DOWN_LEFT, PA_SCROLL_DOWN_RIGHT,
#endif // ENA_SCR_DIA
#if ENA_WIPE
  PA_WIPE, PA_WIPE_CURSOR,
#endif  // ENA_WIPES
#if ENA_OPNCLS
  PA_OPENING, PA_OPENING_CURSOR, PA_CLOSING, PA_CLOSING_CURSOR, 
#endif // ENA_OPNCLS
#if ENA_GROW
  PA_GROW_UP, PA_GROW_DOWN, 
#endif // ENA_GROW
#if ENA_MISC
  PA_SLICE, PA_MESH, PA_FADE, PA_DISSOLVE, PA_BLINDS, PA_RANDOM,
#endif //ENA_MISC
#if ENA_SCAN
  PA_SCAN_HORIZ, PA_SCAN_VERT,
#endif // ENA_SCAN
};

// Global Variables ------------------

// Parola with HARDWARE SPI
// MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


#if USE_ALTSOFTSERIAL
AltSoftSerial BTChan = AltSoftSerial();
#else
SoftwareSerial BTChan = SoftwareSerial(BT_RECV_PIN, BT_SEND_PIN);
#endif

#define SIG0 0x55
#define SIG1 0xaa

struct globalData {
  uint8_t signature[2]; // recognise this as a valid setup

  uint16_t scrollSpeed; // frame delay value in ms
  uint16_t scrollPause; // msg pause in ms

  uint8_t effectI;      // in effect index into table
  uint8_t effectO;      // out effect index into table
  uint8_t align;        // justification index into table

  uint8_t intensity;    // display intensity
  bool bInvert;         // display inverted

  bool doubleH;         // Text size

  char msg[BUF_SIZE+1]; // the message being displayed
  char bottomMsg[BUF_SIZE+1];
} G;

bool doubleHeight = false;

// Global message buffers shared by BT and Scrolling functions
char newMessage[BUF_SIZE+1] = { '\0' };
bool newMessageAvailable = false;

char newBMessage[BUF_SIZE+1] = { '\0' };
bool newBMessageAvailable = false;

bool newConfigAvailable = false;


char messageToShow[BUF_SIZE+1];
char messageBToShow[BUF_SIZE+1];

// Application Code ------------------
void(*hwReset) (void) = 0; //declare reset function @ address 0

void writeGlobal(void)
{
  PRINTS("\nSaving Global");
  EEPROM.put(EEPROM_START_ADDR, G);
}

void readGlobal(bool bInit = false)
{
  PRINTS("\nLoading Global");
  EEPROM.get(EEPROM_START_ADDR, G);

  

  if (bInit || G.signature[0] != SIG0 || G.signature[1] != SIG1)
  // set the default parameters
  {
    PRINTS("\nInitialising Global");
    G.signature[0] = SIG0;
    G.signature[1] = SIG1;
    G.scrollSpeed = 25;
    G.scrollPause = 10;
    G.effectI = G.effectO = PA_SCROLL_LEFT;
    G.align = 1;
    G.intensity = 0;
    G.bInvert = false;
    G.doubleH = true;
    strcpy(G.msg, "Esperando");
    strcpy(G.bottomMsg, "Mensaje");

    writeGlobal();
  }

  fillText(G.msg, G.bottomMsg);
  setTextAnimation();

  newConfigAvailable = true;
}

bool BT_getATCmd(char* szBuf, uint8_t lenBuf, bool fReset = true)
// Copy the AT command from PROGMEM into the buffer provided
// The first call should reset the index counter
// Return true if this is the last command
{
  static uint16_t cmdIdx;

  if (fReset) cmdIdx = 0;

  strncpy_P(szBuf, ATCmd + cmdIdx, lenBuf);
  cmdIdx += (strlen_P(ATCmd + cmdIdx) + 1);

  return(pgm_read_byte(ATCmd + cmdIdx) == '\0');
}

bool BT_getATResponse(char* resp, uint8_t lenBuf)
// Get an AT response from the BT module or time out waiting
{
  const uint16_t RESP_TIMEOUT = 500;

  uint32_t timeStart = millis();
  char c = '\0';
  uint8_t len = 0;

  *resp = '\0';
  while ((millis() - timeStart < RESP_TIMEOUT) && (c != '\n') && (len < lenBuf))
  {
    if (BTChan.available())
    {
      c = BTChan.read();
      *resp++ = c;
      *resp = '\0';
      len++;
    }
  }

  return(len != '\0');
}

void BT_sendACK(char resp)
// Send a protocol ACK to the BT master
{
  static char msg[] = { PKT_START, PKT_CMD_ACK, PKT_ERR_OK, PKT_END, '\n', '\0' };
  msg[2] = resp;
  PRINT("\nResp: ", msg);
  BTChan.print(msg);
  BTChan.flush();
}

void BT_begin(void)
// initialise the BT device for different hardware
{
  const uint16_t BAUD = 9600;
  char  szCmd[20], szResp[16];
  uint8_t   i = 0;
  bool  fLast = false;

  PRINT("\nStart BT connection at ", BAUD);
  BTChan.begin(BAUD);

#if HW_USE_HC05
  // Switch the HC05 to setup mode using digital I/O
  pinMode(HC05_SETUP_ENABLE, OUTPUT);
  digitalWrite(HC05_SETUP_ENABLE, HIGH);
  delay(10);   // just a small amount of time
  digitalWrite(HC05_SETUP_ENABLE, LOW);
#endif

  // Process all the AT commands for the selected BT module
  // Send each command, read the response (or time out) and then
  // do the next one.
  // First item is always the name!
  do
  {
    fLast = BT_getATCmd(szCmd, ARRAY_SIZE(szCmd), (i == 0));

    // Print the preamble, AT command, end of line by assembling the 
    // data into a string of allocated memory. This allows the data to
    // send out in one hit rather than piecemeal.
    char *sz = (char *)malloc((strlen(szStart) + strlen_P(szCmd) + \
                strlen(BT_NAME) + strlen(szEnd) + 1) * sizeof(char));
    strcpy(sz, szStart);
    strcat(sz, szCmd);
    if (i == 0)  // first item - insert the name
      strcat(sz, BT_NAME);
    strcat(sz, szEnd);
    BTChan.print(sz);
    BTChan.flush();

    free(sz);
    i++;

    // Wait for and get the response, except for the 
    // last one when we don't care as normally a RESET.
    if (!fLast)
    {
      if (!BT_getATResponse(szResp, ARRAY_SIZE(szResp)))
      {
        PRINT("\nBT err on ", szCmd);
        PRINT(":", szResp);
      }
    }
  } while (!fLast);

  BTChan.flush();
}

uint8_t BT_executeCommand(uint8_t cmd, char *pd)
{
  uint8_t sts = PKT_ERR_OK;  // assume all ok ...

  PRINT("\nexecuting ", (char)cmd);
  PRINT(" with '", pd);
  PRINTS("'");

  switch (cmd)
  {
  case PKT_CMD_RESET:
    hwReset();
    break;

  case PKT_CMD_FACSET:
    readGlobal(true);
    PRINTS("\nCMD Factory Settings");
    break;

  case PKT_CMD_BRIGHT:
    G.intensity = atoi(pd);
    PRINT("\nCMD Brightness ", G.intensity);
    break;

  case PKT_CMD_IANIM:
    G.effectI = (G.effectI + 1) % ARRAY_SIZE(textEffect);
    PRINT("\nCMD In Animation ", G.effectI);
    break;

  case PKT_CMD_OANIM:
    G.effectO = (G.effectO + 1) % ARRAY_SIZE(textEffect);
    PRINT("\nCMD Out Animation ", G.effectO);
    break;

  case PKT_CMD_INVERT:
    G.bInvert = !P.getInvert();
    PRINT("\nCMD Invert ", G.bInvert ? "on" : "off");
    break;

  case PKT_CMD_JUSTIFY:
    G.align = (G.align + 1) % ARRAY_SIZE(textPosition);
    PRINT("\nCMD Text Align ", G.align);
    break;

  case PKT_CMD_SAVE:
    writeGlobal();
    PRINTS("\nCMD Save");
    break;

  case PKT_CMD_MESSAGE:
    strcpy(newMessage, pd);
    newMessageAvailable = true;
    PRINT("\nCMD Message '", newMessage); PRINTS("'");
    break;

  case PKT_CMD_BMESSAGE:
    strcpy(newBMessage, pd);
    newBMessageAvailable = true;
    PRINT("\nCMD Message '", newBMessage); PRINTS("'");
    break;

  case PKT_CMD_SPEED:
  case PKT_CMD_TPAUSE:
  {
      int16_t v = atoi(pd);

      if (v < 0 || v > 10000)
        sts = PKT_ERR_DATA;
      else
      {
        if (cmd == PKT_CMD_SPEED)
        {
          G.scrollSpeed = v;
          PRINT("\nCMD Speed ", v);
        }
        else
        {
          G.scrollPause = v;
          PRINT("\nCMD Pause ", v);
        }
      }
    }
    break;
  case PKT_CMD_HEIGHT:
    G.doubleH = !G.doubleH;

    setTextAnimation();

    newConfigAvailable = true;
    P.displayReset();
    break;

  default:
    sts = PKT_ERR_CMD;
    PRINT("CMD Error - ", cmd);
    break;
  }

  // set global flags
  newConfigAvailable = (sts == PKT_ERR_OK && cmd != PKT_CMD_MESSAGE);

  return(sts);
}

void BT_getCommand(void)
// Call repeatedly to receive and process characters waiting in the serial queue
// Return true when a good message is fully received
{
  static enum { ST_IDLE, ST_CMD, ST_DATA, ST_END } state = ST_IDLE;
  static uint32_t timeStart = 0;
  static char cBuf[BUF_SIZE+1];
  static uint16_t countBuf;
  static char cmd = '\0';
  static bool prevEsc, inEsc = false;

  // check for timeout if we are currently mid-packet
  if (state != ST_IDLE)
  {
    if (millis() - timeStart >= BT_COMMS_TIMEOUT)
    {
      BT_sendACK(PKT_ERR_TOUT);
      timeStart = 0;
      state = ST_IDLE;
    }
  }

  // process the next character if there is one
  if (BTChan.available())
  {
    char ch = BTChan.read();

    // deal with the escape sequence indicator
    prevEsc = inEsc;
    inEsc = (ch == PKT_ESC && !prevEsc);

    // now run the FSM
    switch (state)
    {
    case ST_IDLE:  // waiting start character
      if (!prevEsc && ch == PKT_START)
      {
        PRINT("\nPkt Srt ", ch);
        state = ST_CMD;
        cmd = cBuf[0] = '\0';
        timeStart = millis();
        countBuf = 0;
      }
      break;

    case ST_CMD:  // reading command
      cmd = ch;   // save the command until we have a full protocol packet
      PRINT("\nPkt Cmd ", cmd);

      switch (ch)
      {
      case PKT_CMD_RESET:
      case PKT_CMD_FACSET:
      case PKT_CMD_BRIGHT:
      case PKT_CMD_IANIM:
      case PKT_CMD_OANIM:
      case PKT_CMD_INVERT:
      case PKT_CMD_JUSTIFY:
      case PKT_CMD_SAVE:
      case PKT_CMD_MESSAGE:
      case PKT_CMD_BMESSAGE:
      case PKT_CMD_SPEED:
      case PKT_CMD_TPAUSE:
      case PKT_CMD_SCROLL:
      case PKT_CMD_HEIGHT:
        state = ST_DATA;
        break;

      default:
        BT_sendACK(PKT_ERR_CMD);
        cmd = '\0';
        state = ST_IDLE;
        break;
      }
      break;

    case ST_DATA:  // reading data
      if (countBuf >= BUF_SIZE) // message too large - stop processing it now
      {
        PRINTS("\nBuffer overflow");
        BT_sendACK(PKT_ERR_DATA);
        state = ST_IDLE;
      }
      else if (ch == PKT_END && !prevEsc) // end character not escaped
      {
        PRINT("\nPkt end @", countBuf);
        cBuf[countBuf] = '\0'; // terminate the string
        BT_sendACK(BT_executeCommand(cmd, cBuf));
        state = ST_IDLE;
      }
      else if (!inEsc)  // not escaping, so save this
      {
        PRINT("\nPkt cBuf[", countBuf); PRINT("]:", ch);
        cBuf[countBuf++] = ch;
      }
      break;

    default:  // something screwed up - reset the FSM
      state = ST_IDLE;
      BT_sendACK(PKT_ERR_SEQ);
      break;
    }
  }
}



void setup()
{
#if DEBUG
  Serial.begin(9600);
#endif
  PRINTS("\n[Parola_BT_Control Debug]");

  readGlobal();

  P.begin(NUM_ZONES);
  setTextAnimation();
  
  P.displayClear();
  
  BT_begin();

  
  


  invertUpperZone = (HARDWARE_TYPE == MD_MAX72XX::GENERIC_HW || HARDWARE_TYPE == MD_MAX72XX::PAROLA_HW);
  if (invertUpperZone)
  {
#if SCROLL_LEFT // invert and scroll left
    scrollUpper = PA_SCROLL_RIGHT;
    scrollLower = PA_SCROLL_LEFT;
#else           // invert and scroll right
    scrollUpper = PA_SCROLL_LEFT;
    scrollLower = PA_SCROLL_RIGHT;
#endif
  }
  else // not invert
  {
#if SCROLL_LEFT // not invert and scroll left
    scrollUpper = PA_SCROLL_LEFT;
    scrollLower = PA_SCROLL_LEFT;
#else           // not invert and scroll right
    scrollUpper = PA_SCROLL_RIGHT;
    scrollLower = PA_SCROLL_RIGHT;
#endif
  }
}

void loop()
{
  // get the next thing from the BT channel
  BT_getCommand();

  static uint8_t cycle = 0;
  // if the config has changed, need to change the display parameters
  if (newConfigAvailable)
  {
    PRINTS("\nSetting new config");
    P.setSpeed(G.scrollSpeed);
    P.setPause(G.scrollPause);
    // P.setTextEffect(textEffect[G.effectI], textEffect[G.effectO]);
    // P.setTextEffect(ZONE_LOWER, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    //P.setTextEffect(ZONE_UPPER, PA_NO_EFFECT, PA_NO_EFFECT);

    P.setTextAlignment(textPosition[G.align]);
    P.setIntensity(G.intensity);
    P.setInvert(G.bInvert);


    P.displayReset();
    newConfigAvailable = false;
  }

  // if we have a new message, copy it over
  if (newMessageAvailable){
    PRINTS("\nSetting new message");

    strcpy(G.msg, newMessage);

    fillText(G.msg, G.bottomMsg);

    setTextAnimation();
    P.displayReset();
    
    newMessageAvailable = false;
  }

  if (newBMessageAvailable){
    PRINTS("\nSetting new message");

    uint8_t spaces = 0;
    int i;

    strcpy(G.bottomMsg, newBMessage);

    fillText(G.msg, G.bottomMsg);

    setTextAnimation();
    P.displayReset();
    
    newBMessageAvailable = false;
  }



  // keep the animation going in all cases
  if (P.displayAnimate())
    P.displayReset();

}


void fillText(char *text1, char *text2){

  strcpy(messageToShow, G.msg);
  strcpy(messageBToShow, G.bottomMsg);

  uint8_t spaces = 0;
  int i;

  if (strlen(text1) > 0 && strlen(text1) < strlen(text2)) {
    spaces = strlen(text2) - strlen(text1);


    for(i = strlen(text1); i < BUF_SIZE && spaces > 0; i++){
      messageToShow[i] = '_';
      spaces--;
    }

    messageToShow[i] = '\0';
    messageToShow[BUF_SIZE] = '\0';

  }else if(strlen(text1) > 0 && strlen(text1) > strlen(text2)){
    spaces = strlen(text1) - strlen(text2);

    for(i = strlen(text2); i < BUF_SIZE && spaces > 0; i++){
      messageBToShow[i] = '_';
      spaces--;
    }

    messageBToShow[i] = '\0';
    messageBToShow[BUF_SIZE] = '\0';
  }
}

void setTextAnimation() {
  
  if (G.doubleH) {
    displayDoubleHeightText(G.msg);
  } else {
    P.setZone(ZONE_LOWER, 0, MAX_DEVICES / 2 - 1);
    P.setZone(ZONE_UPPER, MAX_DEVICES / 2, MAX_DEVICES - 1);

    P.setFont(nullptr);
    P.setCharSpacing(1);

    P.displayZoneText(ZONE_LOWER, messageBToShow, G.align, G.scrollSpeed, G.scrollPause, PA_SCROLL_LEFT, PA_SCROLL_LEFT); // We use only the left scroll for simplicity and to avoid errors
    P.displayZoneText(ZONE_UPPER, messageToShow, G.align, G.scrollSpeed, G.scrollPause, PA_SCROLL_LEFT, PA_SCROLL_LEFT); // idem

    // if (scrolling) {
    //} else {
    //  P.displayText(G.msg, PA_CENTER, SCROLL_SPEED, PAUSE_TIME, PA_BLINDS, PA_BLINDS);
    //}
  }

  P.displayClear();
  P.displayReset();
}

void displayDoubleHeightText(const char* msg) {
  char upperMsg[BUF_SIZE+1];
  char lowerMsg[BUF_SIZE+1];

  // Generar el mensaje para la parte superior e inferior
  for (int i = 0; i < strlen(msg); i++) {
    lowerMsg[i] = msg[i];
    upperMsg[i] = msg[i] + 128;  // Agregar 128 para la parte superior
  }
  lowerMsg[strlen(msg)] = '\0';
  upperMsg[strlen(msg)] = '\0';

  
  // Configurar zonas para texto de doble altura
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE - 1);
  P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES - 1);

  P.setFont(BigFont);
  P.setCharSpacing(2);

  if (invertUpperZone)
  {
    P.setZoneEffect(ZONE_UPPER, true, PA_FLIP_UD);
    P.setZoneEffect(ZONE_UPPER, true, PA_FLIP_LR);
  }

  P.displayClear(ZONE_LOWER);
  P.displayClear(ZONE_UPPER);

  P.displayZoneText(ZONE_LOWER, lowerMsg, G.align, G.scrollSpeed, G.scrollPause, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  P.displayZoneText(ZONE_UPPER, upperMsg, G.align, G.scrollSpeed, G.scrollPause, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  P.synchZoneStart();
}


