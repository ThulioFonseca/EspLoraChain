
// ----------------------------------- [ EspLoRa Transmitter ] ---------------------------------------- //
//
//   Autor: Thulio Fonseca
//   Curso: Sistemas de Informação - IFMG - Ouro Branco
//   Descrição: Dispositivo IoT para coleta e transmição de dados inerentes à qualidade do Leite. 
//      

// --------------------------------- [ Declaração de Bibliotecas ] ------------------------------------ //

#include <SoftwareSerial.h>
#include <OneWire.h>  
#include <DallasTemperature.h>
#include <RTClib.h>
#include <ArduinoJson.h>

// --------------------------------- [ Definição de nomes para pinos de I/O ] ------------------------- //

#define LedStatus 02
#define TxPin 15
#define RxPin 13
#define SensorTempPin 00

// --------------------------------- [ Instânciação de Objetos ] -------------------------------------- //

SoftwareSerial LoRaSerial(RxPin,TxPin);
OneWire onewire(SensorTempPin);
DallasTemperature SensorTemperatura(&onewire);
RTC_DS1307 DS1307_RTC;

// --------------------------------- [ Declaração de variáveis globais ] ------------------------------- //

uint32_t deviceId = ESP.getChipId();
bool comunicacaoEstabelecida = false;
bool SerialLogStatus = 0;

// ------------------------------------------ [ Setup ] ------------------------------------------------ //

void setup() {
  
// Definição de funções dos pinos I/O:

  pinMode(RxPin, INPUT);
  pinMode(TxPin, OUTPUT);
  pinMode(LedStatus, OUTPUT);
  pinMode(SensorTempPin, INPUT);
  
// Inicialização da comunicação Serial e I2C:

  Serial.begin(9600);
  Serial.flush();
  
  LoRaSerial.begin(9600);
  LoRaSerial.flush();    

  if (!DS1307_RTC.begin()) {
    SerialLog("Não foi possivel iniciar o RTC", SerialLogStatus);
    while(1);
  }

// Ajuste inicial de relógio:

  DS1307_RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));  
  delay(2000);

  SerialLog("Módulo EspLoRa TX iniciado", SerialLogStatus);

// Buscar contato inicial com Receptor:

  comunicacaoEstabelecida = BuscarConexao();

}

// ------------------------------------------ [ Loop ] ------------------------------------------------ //

void loop() {
 
 while(comunicacaoEstabelecida){

    //Coleta de Dados dos Sensores:
    
    DateTime now = DS1307_RTC.now();
    int aguardaResposta = 0;
    float phSensor = random(600, 700) / 100.0;  
    SensorTemperatura.requestTemperatures();
    float tempSensor = SensorTemperatura.getTempCByIndex(0);

    // Criação do "Payload" JSON:
    
    StaticJsonDocument<200> doc;
    doc["timestamp"] = now.timestamp();
    doc["deviceId"] = deviceId;
    doc["ph"] = serialized(String(phSensor,2));
    doc["temperatura"] = serialized(String(tempSensor,2)); 

    // Envio do Payload via LoRa:
    
    serializeJson(doc, LoRaSerial);
    LoRaSerial.flush();   
    SerialLog("Transmitindo dados: ", SerialLogStatus);         
    
    // Aguardando Resposta do receptor:
    
    while(!LoRaSerial.available() >= 1 && aguardaResposta < 30) {
      digitalWrite(LedStatus, HIGH);
      SerialLog("Conexão perdida - aguardando " + (String)aguardaResposta, SerialLogStatus);
      delay(250);
      aguardaResposta++;      
    }

    // Verifica se o tempo limite de espera de conexão foi atingido:
    
    if(aguardaResposta >= 27){
      comunicacaoEstabelecida = false;
      continue;
    }

    // Lê a resposta do Receptor
    
    digitalWrite(LedStatus, LOW);
    String respostaReceptor = LoRaSerial.readString();
    SerialLog("Resposta: " + respostaReceptor, SerialLogStatus);     
    
  }

  //Tenta reconetar:
  comunicacaoEstabelecida = BuscarConexao();
    
}

bool BuscarConexao(){
    
  bool sucessoConexao = false;
  int tentativa = 0;
  int aguardarResposta = 0;
  String respostaReceptor = "no conection";
  digitalWrite(LedStatus, HIGH); 
   
  while(!sucessoConexao){

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
      
      respostaReceptor = LoRaSerial.readString();
      LoRaSerial.flush();
      
      if(respostaReceptor == "pong"){
        sucessoConexao = true;
        SerialLog("Comunicação estabelecida!", SerialLogStatus);       
        digitalWrite(LedStatus, LOW); 
      }
      else                
        SerialLog("Resposta inválida: " + respostaReceptor, SerialLogStatus);  
        LoRaSerial.flush();            
    }
    
    aguardarResposta = 0; 
    tentativa++;

  }
  
  return sucessoConexao;
    
}

void SerialLog(String message, bool habilitado){

  if(habilitado){
    Serial.println(message);
  }
}
