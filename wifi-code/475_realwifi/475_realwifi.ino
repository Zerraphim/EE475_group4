#include<SoftwareSerial.h>
SoftwareSerial client(2,3); //RX, TX

String webpage="";
int i=0,k=0;
String readString;
int x=0;
////////////////////////////////////////////////////
//////////PARAMETERS HERE///////////////////////////
////////////////////////////////////////////////////
String temp = "0";
String heartrate = "0";
int fall = 0;

////////////////////////////////////////////////////
////////////////////////////////////////////////////
boolean No_IP=false;
String IP="";
char temp1='0';

String name="<p>Circuit Digest</p>";   //22
String dat="<p>Data Received Successfully.....</p>";     //21
     
void check4IP(int t1)
{
  int t2=millis();
  while(t2+t1>millis())
  {
    while(client.available()>0)
    {
      if(client.find("WIFI GOT IP"))
      {
        No_IP=true;
      }
    }
  }
}

void get_ip()
{
  IP="";
  char ch=0;
  while(1)
  {
    client.println("AT+CIFSR");
    while(client.available()>0)
    {
      if(client.find("STAIP,"))
      {
        delay(1000);
        Serial.print("IP Address:");
        while(client.available()>0)
        {
          ch=client.read();
          if(ch=='+')
          break;
          IP+=ch;
        }
      }
      if(ch=='+')
      break;
    }
    if(ch=='+')
    break;
    delay(1000);
  }
  Serial.print(IP);
  Serial.print("Port:");
  Serial.println(80);
}

void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    Serial.println(cmd);
    client.println(cmd); 
    while(client.available())
    {
      if(client.find("OK"))
      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  Serial.println("OK");
  else
  Serial.println("Error");
}

void wifi_init()
{
      connect_wifi("AT",100);
      connect_wifi("AT+CWMODE=3",100);
      connect_wifi("AT+CWQAP",100);  
      connect_wifi("AT+RST",5000);
      check4IP(5000);
      if(!No_IP)
      {
        Serial.println("Connecting Wifi....");
        connect_wifi("AT+CWJAP=\"CenturyLink1521\",\"rm682pf5y7ymny\"",7000);         //provide your WiFi username and password here
     // connect_wifi("AT+CWJAP=\"vpn address\",\"wireless network\"",7000);
      }
      else
        {
        }
      Serial.println("Wifi Connected"); 
      get_ip();
      connect_wifi("AT+CIPMUX=1",100);
      connect_wifi("AT+CIPSERVER=1,80",100);
}

void sendwebdata(String webPage)
{
    int ii=0;
     while(1)
     {
      unsigned int l=webPage.length();
      Serial.print("AT+CIPSEND=0,");
      client.print("AT+CIPSEND=0,");
      Serial.println(l+2);
      client.println(l+2);
      delay(100);
      Serial.println(webPage);
      client.println(webPage);
      while(client.available())
      {
        //Serial.print(Serial.read());
        if(client.find("OK"))
        {
          ii=11;
          break;
        }
      }
      if(ii==11)
      break;
      delay(100);
     }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////The codes that needs to be put into the setup block//////////////////////////////////////////////
void setup() 
{
   Serial.begin(9600);
   client.begin(115200);
   wifi_init();
   Serial.println("System Ready..");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////The codes(inside the while loop which is always true) that needs to be put into the infinite loop
void loop() 
{
    while (1) {
     Serial.println("Please Refresh your Page");
     delay(1000);
     while(client.available()){
      if(client.find("0,CONNECT"))
      {
        Serial.println("Start Printing");
        Send();
        Serial.println("Done Printing");
        delay(1000);
      }
    }
    delay(1);
   }
}

void Send()
{
    if (fall == 1) { //There is a fall detected
      webpage = "<table style=\"width:100%;border: 1px solid black;\"><tr><th style=\"border: 1px solid black;\">Heartrate</th><th>Temperature</th><th style=\"border: 1px solid black;\">Fall?</th></tr><tr><td>" + heartrate + "</td><td style=\"border: 1px solid black;\">" + temp + "</td><td>safe</td></tr></table>";
      sendwebdata(webpage);
    } else {
       webpage = "<table style=\"width:100%;border: 1px solid black;\"><tr><th style=\"border: 1px solid black;\">Heartrate</th><th>Temperature</th><th style=\"border: 1px solid black;\">Fall?</th></tr><tr><td>" + heartrate + "</td><td style=\"border: 1px solid black;\">" + temp + "</td><td>DANGER!</td></tr></table>";
      sendwebdata(webpage);
    }
      delay(1000);
      client.println("AT+CIPCLOSE=0"); 
}
