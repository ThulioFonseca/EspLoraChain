#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

SSD1306Wire oled(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst

#define RF_FREQUENCY               915000000 // Hz
#define TX_OUTPUT_POWER            14        // dBm
#define LORA_BANDWIDTH             0         // [0: 125 kHz, 1: 250 kHz,2: 500 kHz,3: Reserved]
#define LORA_SPREADING_FACTOR      7         // [SF7..SF12]
#define LORA_CODINGRATE            1         // [1: 4/5, 2: 4/6, 3: 4/7,4: 4/8]
#define LORA_PREAMBLE_LENGTH       8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT        0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON       false
#define RX_TIMEOUT_VALUE           1000
#define BUFFER_SIZE                30 // Define the payload size here

#define LDR 7 // Sensor de Luz

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
double txNumber;
bool lora_idle = true;
static RadioEvents_t RadioEvents;
void OnTxDone( void );
void OnTxTimeout( void );

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialising the UI will init the display too.
  oled.init();
  oled.setFont(ArialMT_Plain_10);
  oled.setColor(WHITE);
  
  Mcu.begin();

  txNumber = 0;

  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;

  Radio.Init( &RadioEvents );
  Radio.SetChannel( RF_FREQUENCY );
  Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                     LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                     LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                     true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
}



void loop()
{
  if (lora_idle == true)
  {
    delay(1000);
    //txNumber += 0.01;

    int luminosidade = analogRead(LDR);
    sprintf(txpacket, "{ \"Luz\": \"%i\"}", luminosidade); //start a package

    Serial.printf("\r\nPacote enviado \"%s\" , length %d\r\n", txpacket, strlen(txpacket));

    Radio.Send( (uint8_t *)txpacket, strlen(txpacket) ); //send the package out
    lora_idle = false;
  }
  Radio.IrqProcess( );
}

void OnTxDone( void )
{
  Serial.println("TX done......");
  lora_idle = true;
  oled.clear();
  oled.setFont(ArialMT_Plain_10);
  oled.setTextAlignment(TEXT_ALIGN_LEFT);
  oled.drawStringMaxWidth(0, 0, 128,"Pacote enviado:");
  oled.drawStringMaxWidth(0, 13, 128,String(txpacket) );
  oled.display();
}

void OnTxTimeout( void )
{
  Radio.Sleep( );
  Serial.println("TX Timeout......");
  lora_idle = true;
}
