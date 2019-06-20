#include <Arduino.h>
#include <heltec.h>

#define BAND 433E6
#define btn 13

String message = "FOXTROT";

byte localAddress = 0xB1;
byte destination = 0xFF;

byte msgCount = 0;
long lastSendTime = 0;
int interval = 2000;

//Prototipos das funções
void readButton();
void messageSent();
void lengthErrorText();
void receptorErrorText();
void loRaData(String sender, String incoming);
void onReceive(int packetSize);
void sendMessage(String outgoing);

void setup()
{
  pinMode(btn, OUTPUT);

  Heltec.begin(true, true, true, true, BAND);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 0, "LoRa INICIADO");
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(10, 16, "Aguardando dados...");
  Heltec.display->display();
  delay(2000);
}

void loop()
{
  if (millis() - lastSendTime > interval)
  {
    readButton();
    onReceive(Heltec.LoRa.parsePacket());
  }
}

void sendData(String msg)
{
  Heltec.LoRa.beginPacket();
  Heltec.LoRa.write(destination);
  Heltec.LoRa.write(localAddress);
  Heltec.LoRa.write(msgCount);
  Heltec.LoRa.write(message.length());
  Heltec.LoRa.print(message);
  if (LoRa.endPacket())
  { //retorno= 1:sucesso | 0: falha
    messageSent();
    msgCount++;
  }
}

void onReceive(int packetSize)
{
  if (packetSize == 0)
    return;

  int recipient = Heltec.LoRa.read();
  byte sender = Heltec.LoRa.read();
  byte incomingMsgId = Heltec.LoRa.read();
  byte incomingLength = Heltec.LoRa.read();

  String incoming = "";

  while (Heltec.LoRa.available())
  {
    incoming += (char)Heltec.LoRa.read();
  }

  if (incomingLength != incoming.length())
  {
    return lengthErrorText();
  }

  if (recipient != localAddress && recipient != 0xFF)
  {
    return receptorErrorText();
  }

  return loRaData(String(sender, HEX), incoming);
}

void readButton()
{
  if (digitalRead(btn))
  {
    lastSendTime = millis();
    interval = random(2000) + 2000;
    sendData(message);
  }
}

void loRaData(String sender, String incoming)
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "Mensagem de: " + String(sender));
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawStringMaxWidth(10, 20, 128, incoming);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 50, "RSSI: " + String(LoRa.packetRssi()));
  Heltec.display->drawString(60, 50, "SNR: " + String(LoRa.packetSnr()));
  Heltec.display->display();
}

void lengthErrorText()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(55, 0, "ERRO: ");
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawStringMaxWidth(0, 15, 128, "Mensagem diferente do esperado!!");
  Heltec.display->display();
}

void receptorErrorText()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(55, 0, "ERRO: ");
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawStringMaxWidth(0, 15, 128, "Esta mensagem não é pra mim!!!");
  Heltec.display->display();
}

void messageSent()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(10, 10, "MENSAGEM");
  Heltec.display->drawString(12, 30, "ENVIADA");
  Heltec.display->display();
}