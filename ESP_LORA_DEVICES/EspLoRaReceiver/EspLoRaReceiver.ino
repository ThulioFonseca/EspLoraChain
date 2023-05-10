
/****** RX *********/

#include "Arduino.h"
#include "heltec.h"

#define LedStatus 02
#define TxPin 33
#define RxPin 34

bool comunicacaoEstabelecida = false;
bool SerialLogStatus = 0;

void setup() {

  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/);
  Heltec.display->setFont(ArialMT_Plain_10);
  

  pinMode(RxPin, INPUT);
  pinMode(TxPin, OUTPUT);
  pinMode(LedStatus, OUTPUT);

  Serial.begin(9600);
  SerialLog("Comunicacao serial iniciada!", SerialLogStatus);

  Serial2.begin(9600, SERIAL_8N1, 34, 33);
  SerialLog("Módulo LoRa TX iniciado", SerialLogStatus);

  drawProgressBarStartup();

  comunicacaoEstabelecida = messageScan();

}

void loop() {

  while(comunicacaoEstabelecida){

      int aguardaResposta = 0;
            
      while(!Serial2.available() >= 1 && aguardaResposta < 30) {
      
        SerialLog("Aguardando Conexão - " + (String)aguardaResposta, SerialLogStatus);
        delay(250);
        aguardaResposta++;
        
    }

    if(aguardaResposta >= 27){
      comunicacaoEstabelecida = false;
      continue;
    }

    String message = Serial2.readString();
    Serial2.flush();

    if(message == "ping")
      comunicacaoEstabelecida = false;    
    
    SerialLog("Recebendo dados: " + message, SerialLogStatus);
    ShowLogDisplay(message);    

    Serial2.write("ACK");
    Serial2.flush();        
  }

  comunicacaoEstabelecida = messageScan();
}

bool messageScan(){
  
  bool sucesso = false;

  while(!sucesso){  

    ShowLogDisplay("Aguardando ping...");

    if(Serial2.available() >= 1 ){
      
      String message = Serial2.readString();
      Serial2.flush();
  
      if(message == "ping"){
        
        digitalWrite(LedStatus, LOW);      
        Serial2.print("pong");
        Serial2.flush();
        
        sucesso = true;
        SerialLog("Comunicação estabelecida-> " + message + " recebido", SerialLogStatus); 
        ShowLogDisplay("Ping Recebido!!!");          
      }
      else {
        
        digitalWrite(LedStatus, HIGH);
        SerialLog("Aguardando mensagem ping -> " + message, SerialLogStatus);
        ShowLogDisplay("Mensagem Invalida");  
      
      }     
    }
    
    delay(500); 
    
  };
  
  return sucesso; 
  
}

void drawProgressBarStartup() {

  int counter = 1;
  while(counter < 500){
    
  Heltec.display->clear();  
  int progress = (counter / 5) % 100;
  // draw the progress bar
  Heltec.display->drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
  Heltec.display->drawString(64, 15, String(progress) + "%");
  Heltec.display->display();
  
  counter++;
  delay(2);

  }
  Heltec.display->clear();  
}

void ShowLogDisplay(String messageToShow){
  
    Heltec.display->clear();  
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
     Heltec.display->drawStringMaxWidth(0, 0, 128, messageToShow);
    Heltec.display->display();
    
}

void SerialLog(String message, bool habilitado){

  if(habilitado){
    Serial.println(message);
  }
}
