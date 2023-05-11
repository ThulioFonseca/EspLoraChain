// ----------------------------------- [ EspLoRa Receiver ] ---------------------------------------- //
//
//   Autor: Thulio Fonseca
//   Curso: Sistemas de Informação - IFMG - Ouro Branco
//   Descrição: Dispositivo IoT para recepção e retransmição de dados para o Nó Blockchain 
//      

// --------------------------------- [ Declaração de Bibliotecas ] ------------------------------------ //

#include "Arduino.h"
#include "heltec.h"

// --------------------------------- [ Definição de nomes para pinos de I/O ] ------------------------- //

#define LedStatus 35
#define TxPin 33
#define RxPin 34

// --------------------------------- [ Declaração de variáveis globais ] ------------------------------- //

bool comunicacaoEstabelecida = false;
bool SerialLogStatus = 1;

// ------------------------------------------ [ Setup ] ------------------------------------------------ //

void setup() {

// Inicialização do Display OLED 0,96":

  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/);
  Heltec.display->setFont(ArialMT_Plain_10);  

// Definição de funções dos pinos I/O:

  pinMode(RxPin, INPUT);
  pinMode(TxPin, OUTPUT);
  pinMode(LedStatus, OUTPUT);

 // Inicialização da comunicação Serial:

  Serial.begin(9600);
  Serial.flush();
  
  Serial2.begin(9600, SERIAL_8N1, 34, 33);
  Serial2.flush();

  ExibirBarraProgresso();

  comunicacaoEstabelecida = BuscarConexao();

}

// ------------------------------------------ [ Loop ] ------------------------------------------------ //

void loop() {

  while(comunicacaoEstabelecida){  

    int aguardaResposta = 0;

    // Aguardando contato do transmissor:
          
    while(!Serial2.available() >= 1 && aguardaResposta < 30) {        
      analogWrite(LedStatus, 0); 
      delay(250);
      aguardaResposta++;        
    }

     // Verifica se o tempo limite de espera de conexão foi atingido:

    if(aguardaResposta >= 27){
      comunicacaoEstabelecida = false;
      continue;
    }

    // Lê a menssagem do Transmissor:
    
    analogWrite(LedStatus, 48); 
    String message = Serial2.readString();

    // Verifica o conteúdo:
    
    if(message == "ping"){      
      comunicacaoEstabelecida = false;
      continue;
    }          

    // Envia do Payload recebido para Nó Blockchain via Serial:
    
    SerialLog(message, SerialLogStatus);
    ShowLogDisplay(message);    

    // Envia do confirmação via LoRa:
    
    Serial2.write("ACK");
    Serial2.flush();        
  }
  
  //Tenta reconetar:
  comunicacaoEstabelecida = BuscarConexao();
}

bool BuscarConexao(){
  
  bool sucessoConexao = false;

  while(!sucessoConexao){  

    ShowLogDisplay("Aguardando contato...");

    if(Serial2.available() >= 1 ){
      
      String message = Serial2.readString();
      Serial2.flush();
  
      if(message == "ping"){
        
        analogWrite(LedStatus, 48);      
        Serial2.print("pong");
        Serial2.flush();
        
        sucessoConexao = true;
        ShowLogDisplay("Contato Recebido!!!");          
      }
      else {
        
        analogWrite(LedStatus, 0);
        ShowLogDisplay("Mensagem Invalida"); 
      
      }     
    }
    
    delay(500); 
    
  };
  
  return sucessoConexao; 
  
}

void ExibirBarraProgresso() {

  int counter = 1;
  while(counter < 500){
    
  Heltec.display->clear();  
  int progress = (counter / 5) % 100;
  // draw the progress bar
  Heltec.display->drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
  Heltec.display->drawString(64, 15, "Iniciando Receptor - " + String(progress) + "%");
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
