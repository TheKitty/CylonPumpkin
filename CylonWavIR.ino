/*
 * Text-to-speech example to speak the first n digits of pi.
 * The number is stored in flash, each digit is spoken one at a time.
 */
#include <WaveHC.h>
#include <WaveUtil.h>
#include <IRremote.h>

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;   // This object represent the WAV file for a pi digit or period
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
char eyesound[13]="eye2.wav";
int mute = 0;
/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))

// IR Remote code
int RECV_PIN = 9;  // pin 11 used by SD card interface so select pin 9 for IR
IRrecv irrecv(RECV_PIN);
decode_results results;
long lasttime=0, lastcode=0, timediff=0;

//////////////////////////////////// SETUP

void setup() {
  // set up Serial library at 9600 bps
  Serial.begin(9600);           
  
  if (!card.init()) {
    error("Card init. failed!");
  }
  if (!vol.init(card)) {
    error("No partition!");
  }
  if (!root.openRoot(vol)) {
    error("Couldn't open dir");
  }

  irrecv.enableIRIn(); // Start the IR receiver
}

/////////////////////////////////// LOOP

void loop() { 
  if(mute == 0) playcomplete(eyesound);
  // check for keypress happened 
  if (irrecv.decode(&results)) {
     Serial.println(results.value, HEX);
     switch (results.value) {
       case 0x83228B74:    // 1
         playcomplete("command.wav");
         break;
       case 0x83228F70:    // 2
         playcomplete("entertan.wav");
         break;
       case 0x8322906F:    // 3
         playcomplete("extermin.wav");
         break;
       case 0x83228A75:    // 4
         playcomplete("leader.wav");
         break;
       case 0x8322847B:    // 5
         playcomplete("survivor.wav");
         break;  
       case 0x83227887:    // 6
         playcomplete("atention.wav");
         break;
       case 0x8322629D:    // vol up
         mute = 0;         // mute off
         break;  
       case 0x83226E91:    // mute
         { Serial.println("mute detected");
           timediff=millis()-lasttime;
           Serial.println(timediff);
           if(lastcode!=results.value || (lastcode==results.value && (timediff>1600)) ) {
              if( mute == 0 ) { // is mute off?
                 Serial.println("toggle off to on");
                 mute = 1;      // turn on
                 // delay(1000);    // wait a bit for debounce
                 break;
                }
              if( mute == 1 ) { // is mute on?
                 mute = 0;      // turn off
                 Serial.println("toggle on to off");
                 break;
                }
             } // end if
            break;
          } // end case 
       } // end switch
     lastcode = results.value;
     lasttime = millis();
     irrecv.resume();   // Receive the next value
  }
}

/////////////////////////////////// HELPERS

/*
 * print error message and halt
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
 * print error message and halt if SD I/O error
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
/*
 * Play a file and wait for it to complete
 */
void playcomplete(char *name) {
  playfile(name);
  while (wave.isplaying);
  
  // see if an error occurred while playing
  sdErrorCheck();
}
/*
 * Open and start playing a WAV file
 */
void playfile(char *name) {
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  if (!file.open(root, name)) {
    PgmPrintln("Couldn't open file ");
    Serial.print(name); 
    return;
  }
  if (!wave.create(file)) {
    PgmPrintln("Not a valid WAV");
    return;
  }
  // ok time to play!
  wave.play();
}
