
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

void setup() {

  pinMode(RxPin, INPUT);
  pinMode(TxPin, OUTPUT);
  pinMode(LedStatus, OUTPUT);
  pinMode(SensorTempPin, INPUT);
  
  Serial.begin(9600);
  Serial.println("Comunicacao serial iniciada!");

  LoRaSerial.begin(9600);
  Serial.println("Módulo LoRa TX iniciado");

  if (!DS1307_RTC.begin()) {
    Serial.println("Não foi possivel iniciar o RTC");
    while(1);
  }
  
  //DS1307_RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  delay(2000);

  comunicacaoEstabelecida = ReceiverSearch();

}

void loop() {
 
 while(comunicacaoEstabelecida){

    String payload = "";
    String phSensor = "Sensor ph: 6.3;";
    String tempSensor = "Sensor Temperatura: ";

    DateTime now = DS1307_RTC.now();
    
    SensorTemperatura.requestTemperatures();
    float temperatura = SensorTemperatura.getTempCByIndex(0);
    tempSensor.concat(temperatura);
    
    payload = now.timestamp()+ "-" + device + phSensor + tempSensor;

    Serial.println("Transmitindo dados: " + payload);     
    LoRaSerial.print(payload);
    
    while(!LoRaSerial.available()){}

    String response = LoRaSerial.readString();
    Serial.println("Resposta: " + response);  

  }    
    
}


bool ReceiverSearch(){
  
  bool sucesso = false;
  int tentativa = 0;
  int aguardarResposta = 0;
  String response = "no conection";
  digitalWrite(LedStatus, HIGH); 

   
  while(!sucesso){

    Serial.println("Enviando  ping, tentativa - " + (String)tentativa);   
    LoRaSerial.print("ping");

    while(!LoRaSerial.available() || aguardarResposta < 20){

      Serial.println("Aguardando resposta do receptor - " + (String)aguardarResposta);   
      aguardarResposta++;      
      delay(250); 
      
    }
        
    if(LoRaSerial.available()){
      
      response = LoRaSerial.readString();
      LoRaSerial.flush();
      
      if(response == "pong"){
        sucesso = true;
        Serial.println("Comunicação estabelecida!");     
        digitalWrite(LedStatus, LOW); 
      }
      else                
        Serial.println("Resposta inválida: " + response);            
    }
    
    aguardarResposta = 0; 
    tentativa++;

  }
  
  return sucesso;
    
}
