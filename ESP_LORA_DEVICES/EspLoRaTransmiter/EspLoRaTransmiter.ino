
/****** TX *********/

#include <SoftwareSerial.h>
#include <OneWire.h>  
#include <DallasTemperature.h>
#include <RTClib.h>

#define LedStatus 02
#define TxPin 15
#define RxPin 13
#define SensorTempPin 00

SoftwareSerial LoRaSerial(RxPin,TxPin);
OneWire onewire(SensorTempPin);
DallasTemperature SensorTemperatura(&onewire);
RTC_DS1307 DS1307_RTC;

uint32_t deviceId = ESP.getChipId();
String device = "deviceId: " + (String)deviceId + "; ";
bool comunicacaoEstabelecida = false;
bool SerialLogStatus = 0;

void setup() {

  pinMode(RxPin, INPUT);
  pinMode(TxPin, OUTPUT);
  pinMode(LedStatus, OUTPUT);
  pinMode(SensorTempPin, INPUT);
  
  Serial.begin(9600);
  SerialLog("Comunicacao serial iniciada!", SerialLogStatus);

  LoRaSerial.begin(9600);
  LoRaSerial.flush();
  SerialLog("Módulo LoRa TX iniciado", SerialLogStatus);

  if (!DS1307_RTC.begin()) {
    SerialLog("Não foi possivel iniciar o RTC", SerialLogStatus);
    while(1);
  }
  
  //DS1307_RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  delay(2000);
  
  comunicacaoEstabelecida = ReceiverSearch();

}

void loop() {
 
 while(comunicacaoEstabelecida){

    int aguardaResposta = 0;
    String payload = "";
    String phSensor = "Sensor ph: 6.3;";
    String tempSensor = "Sensor Temperatura: ";

    DateTime now = DS1307_RTC.now();
    
    SensorTemperatura.requestTemperatures();
    float temperatura = SensorTemperatura.getTempCByIndex(0);
    tempSensor.concat(temperatura);
    
    payload = now.timestamp()+ " - " + device + phSensor + tempSensor;
    SerialLog("Transmitindo dados: " + payload, SerialLogStatus);
         
    LoRaSerial.print(payload);
    LoRaSerial.flush();
    
    while(!LoRaSerial.available() >= 1 && aguardaResposta < 30) {
      
      SerialLog("Conexão perdida - aguardando " + (String)aguardaResposta, SerialLogStatus);
      delay(250);
      aguardaResposta++;
      
    }

    if(aguardaResposta >= 27){
      comunicacaoEstabelecida = false;
      continue;
    }

    String receiverResponse = LoRaSerial.readString();
    LoRaSerial.flush();
    SerialLog("Resposta: " + receiverResponse, SerialLogStatus);     
    
  }

  comunicacaoEstabelecida = ReceiverSearch();
    
}


bool ReceiverSearch(){
    
  bool sucesso = false;
  int tentativa = 0;
  int aguardarResposta = 0;
  String receiverResponse = "no conection";
  digitalWrite(LedStatus, HIGH); 
   
  while(!sucesso){

    SerialLog("Enviando  ping, tentativa - " + (String)tentativa, SerialLogStatus);    
    LoRaSerial.print("ping");

    while(!LoRaSerial.available() >=1){

      SerialLog("Aguardando resposta do receptor - " + (String)aguardarResposta, SerialLogStatus);   
      aguardarResposta++;

      if(aguardarResposta > 8) {
        LoRaSerial.print("ping");
        aguardarResposta = 0;        
      }       
        
      delay(250); 
      
    }
        
    if(LoRaSerial.available() >= 1){
      
      receiverResponse = LoRaSerial.readString();
      LoRaSerial.flush();
      
      if(receiverResponse == "pong"){
        sucesso = true;
        SerialLog("Comunicação estabelecida!", SerialLogStatus);       
        digitalWrite(LedStatus, LOW); 
      }
      else                
        SerialLog("Resposta inválida: " + receiverResponse, SerialLogStatus);  
        LoRaSerial.flush();            
    }
    
    aguardarResposta = 0; 
    tentativa++;

  }
  
  return sucesso;
    
}

void SerialLog(String message, bool habilitado){

  if(habilitado){
    Serial.println(message);
  }
}
