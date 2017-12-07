/*******************************************************************
    Exemplo de como integrar o Telegram com Arduino e atomatização
    ligando e desligando o LED.
 *                                                                 *
    Projeto Final de Curso disciplina Internete das Coisas
    Adaptado por:Claudio, Hurgo e Max
 *******************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Inicializa uma conexão Wi-Fi ao roteador
char ssid[] = "IoT";     // Digite o SSID da Rede (name)
char password[] = "12348765"; // Digite a senha da rede

String chatId= "0";

String chatIdLogando = "0";

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // Seu token do bot Telegram (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //Tempo medio entre o comando e a mensagem
long Bot_lasttime;   //Digitalização da ultima mensagem
bool Start = false;

const int ledPin = D1; //Indentifica o Pino que será acionado.
int ledStatus = 0;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    Serial.print(chat_id);
    Serial.print(" ");
    Serial.print(text);
    Serial.println();

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if(chat_id == chatIdLogando){
      if(text == "admin:1234"){ //Login e senha para logar no sistema
        bot.sendMessage(chatId, "Saindo... outra pessoa logou", "Markdown"); //Se outro se conectar 
        chatIdLogando = "0";
        chatId = chat_id;
        bot.sendMessage(chat_id, "Logado com sucesso...", ""); //msg de boas vindas após logar
        String welcome = "Bem-vindo o que deseja fazer??, " + from_name + ".\n";
        welcome += "Escolha um comando Abaixo.\n\n";
        welcome += "/ledon : Para Ligar Led On \n";
        welcome += "/ledoff : Para Desligar Led OFF\n";
        welcome += "/status : Retorna o status do LED\n";
        bot.sendMessage(chat_id, welcome, "Markdown");
        
      }else{
        bot.sendMessage(chat_id, "Usuario ou senha incorretos", ""); //msg de erro
      }
      
    } else if(chatId == chat_id){
      if (text == "/ledon" && chatId == chat_id) {
        digitalWrite(ledPin, HIGH);   // Liga o LED (HIGH tensao alta)
        ledStatus = 1;
        bot.sendMessage(chat_id, "Led Esta Ligado", "");
      }
  
      if (text == "/ledoff" && chatId == chat_id) {
        ledStatus = 0;
        digitalWrite(ledPin, LOW);    // Desliga LED (LOW tensao baixa)
        bot.sendMessage(chat_id, "Led Esta Desligado", "");
      }
      if (text == "/status") {
        if (ledStatus) {
          bot.sendMessage(chat_id, "Led esta Ligado", "");
        } else {
          bot.sendMessage(chat_id, "Led esta Desligado", "");
        }
      }

    } else if (text == "/logar") {
      ledStatus = 0;
      chatIdLogando = chat_id;
      bot.sendMessage(chat_id, "Digite o usuario:senha", "");
    } else{
      bot.sendMessage(chat_id, "Voce nao esta logado. digite /logar", "");
    }


    if (text == "/start") {
      String welcome = "Bem-vindo Claudio o que deseja fazer??, " + from_name + ".\n";
      welcome += "Escolha um comando Abaixo.\n\n";
      welcome += "/ledon : Para Ligar Led Om \n";
      welcome += "/ledoff : Para Desligar Led OFF\n";
      welcome += "/status : Retorna o status do LED\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);

  // Define o modo WiFi para estacao e conecta-se a um AP
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT); // inicializa o ledPin digital como saida.
  delay(10);
  digitalWrite(ledPin, LOW); // inicia o pino como desligado
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
