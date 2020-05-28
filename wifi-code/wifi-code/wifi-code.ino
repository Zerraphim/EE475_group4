#include<SoftwareSerial.h>
SoftwareSerial client(12, 11); //RX, TX

String webpage = "";
int i = 0, k = 0;
String readString;
int x = 0;

boolean No_IP = false;
String IP = "";
char temp1 = '0';

String name = "<p>Circuit Digest</p>"; //22
String dat = "<p>Data Received Successfully.....</p>";   //21

//We give the system t1 miliseconds to check for the IP address, which is going
//to be the website address later.
void check4IP(int t1)
{
  int t2 = millis();
  while (t2 + t1 > millis())
  {
    while (client.available() > 0)
    {
      if (client.find("WIFI GOT IP"))
      {
        //If No_IP is true, we have the IP address. If false, we don't.
        No_IP = true;
      }
    }
  }
}

//To get the IP address.
void get_ip()
{
  IP = "";
  char ch = 0;
  while (1)
  {
    client.println("AT+CIFSR");
    while (client.available() > 0)
    {
      if (client.find("STAIP,"))
      {
        delay(1000);
        Serial.print("IP Address:");
        while (client.available() > 0)
        {
          ch = client.read();
          if (ch == '+')
            break;
          IP += ch;
        }
      }
      if (ch == '+')
        break;
    }
    if (ch == '+')
      break;
    delay(1000);
  }
  Serial.print(IP);
  Serial.print("Port:");
  Serial.println(80);
}

//Send one command to the WIFI. Here is a list of commands:
//"AT"-------------->The wifi module will respond "OK" if the connection is established
//"AT+CWMODE=3"----->The wifi module is changed to mode 3, (host+client mode).
//"AT+CWQAP"-------->Quit the previously connect WIFI.
//...
void connect_wifi(String cmd, int t)
{
  int temp = 0, i = 0;
  while (1)
  {
    Serial.println(cmd);
    client.println(cmd);
    while (client.available())
    {
      if (client.find("OK"))
        i = 8;
    }
    delay(t);
    if (i > 5)
      break;
    i++;
  }
  if (i == 8)
    Serial.println("OK");
  else
    Serial.println("Error");
}


//We need to provide the wifi username and password here.
void wifi_init()
{
  connect_wifi("AT", 100);
  connect_wifi("AT+CWMODE=3", 100);
  connect_wifi("AT+CWQAP", 100);
  connect_wifi("AT+RST", 5000);
  check4IP(5000);
  if (!No_IP)
  {
    Serial.println("Connecting Wifi....");
    //////////////////////////////////////////////////////////////
    //////////////////////////IMPORTANT///////////////////////////
    //////////////////////////////////////////////////////////////
    /////////provide your WiFi username and password here/////////
    connect_wifi("AT+CWJAP=\"Frontier_5\",\"0935985578\"", 7000);
    // connect_wifi("AT+CWJAP=\"vpn address\",\"wireless network\"",7000);
  }
  else
  {
  }
  Serial.println("Wifi Connected");
  get_ip();
  connect_wifi("AT+CIPMUX=1", 100);
  connect_wifi("AT+CIPSERVER=1,80", 100);
}

void sendwebdata(String webPage)
{
  int ii = 0;
  while (1)
  {
    unsigned int l = webPage.length();
    Serial.print("AT+CIPSEND=0,");
    client.print("AT+CIPSEND=0,");
    Serial.println(l + 2);
    client.println(l + 2);
    delay(100);
    Serial.println(webPage);
    client.println(webPage);
    while (client.available())
    {
      //Serial.print(Serial.read());
      if (client.find("OK"))
      {
        ii = 11;
        break;
      }
    }
    if (ii == 11)
      break;
    delay(100);
  }
}

void setup()
{
  Serial.begin(9600);
  client.begin(9600);
  wifi_init();
  Serial.println("System Ready..");
}

void loop()
{
  k = 0;
  Serial.println("Please Refresh your Page");
  while (k < 1000)
  {
    k++;
    while (client.available())
    {
      if (client.find("0,CONNECT"))
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
  webpage = "<h1>TEST page for Health Inspector</h1><body bgcolor=f0f0f0>";
  sendwebdata(webpage);
  client.println("AT+CIPCLOSE=0");
}
