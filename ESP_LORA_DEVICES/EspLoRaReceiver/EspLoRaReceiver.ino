
/****** RX *********/

#include <SoftwareSerial.h>

#define LedStatus 02
#define TxPin 44
#define RxPin 43

bool comunicacaoEstabelecida = false;

SoftwareSerial LoRaSerial(RxPin,TxPin);

void setup() {

  pinMode(RxPin, INPUT);
  pinMode(TxPin, OUTPUT);
  pinMode(LedStatus, OUTPUT);

  Serial.begin(9600);
  Serial.println("Comunicacao serial iniciada!");

  LoRaSerial.begin(9600);
  Serial.println("Módulo LoRa TX iniciado");

  comunicacaoEstabelecida = messageScan();

}

void loop() {

  if(comunicacaoEstabelecida){
   
    String message = LoRaSerial.readString();
    Serial.println("Recebendo dados: " + message);

    if(message.length() > 1)
      LoRaSerial.print("ACK");    
    delay(10);  
  }  
   
}

bool messageScan(){

  bool sucesso = false;

  while(!sucesso){
    
    String message = LoRaSerial.readString();

    if(message == "ping"){
      
      digitalWrite(LedStatus, LOW);      
      LoRaSerial.print("pong");      
      sucesso = true;
      Serial.println("Comunicação estabelecida-> " + message + " recebido");        
    }
    else {
      
      digitalWrite(LedStatus, HIGH);
      Serial.println("Aguardando mensagem ping -> " + message);  
    }

    delay(10);
    
  }

  return sucesso; 
  
}
