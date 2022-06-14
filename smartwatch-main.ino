    #include "DHT.h"
    #include <ESP8266WiFi.h>
    #include <WiFiClientSecure.h>
    #include <UniversalTelegramBot.h>
    #include <ArduinoJson.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClient.h>
    #include <NewPing.h> //hcsr04
    #include <LiquidCrystal_I2C.h>
    #include <Wire.h>
    

    DynamicJsonDocument tarihsaat(1024);

    #define inB 0
    #define inA 13
    
    const char* ssid = "test";
    const char* password = "8163264128";
    
    //#define BOTtoken "2106080187:AAE9yYAs3W4pR1Lp4fcPxvEdEMYqmDV-XqI"  // your Bot Token (Get from Botfather)
    #define BOTtoken " // your Bot Token (Get from Botfather)" //akillisaat1
    #define CHAT_ID " // your chat id"

    String alarmSaat,tarih,temp;
    int uzaklik=200,uzaklikSayac=0;
    
    JsonObject saatObje;
    #define DHTPIN 2     
    #define DHTTYPE DHT11
    DHT dht(DHTPIN, DHTTYPE);
    
    #ifdef ESP8266
      X509List cert(TELEGRAM_CERTIFICATE_ROOT);
    #endif
    
    WiFiClientSecure client;
    UniversalTelegramBot bot(BOTtoken, client);
    
    const int trigPin = 12;
    const int echoPin = 14;
    
    //define sound velocity in cm/uS
    #define SOUND_VELOCITY 0.034
    #define CM_TO_INCH 0.393701
    
    LiquidCrystal_I2C ekran(0x27,16,2);
    
    // Checks for new messages every 1 second.
    int botRequestDelay = 1000;
    unsigned long lastTimeBotRan;
    
    const int ledPin = 2;
    bool ledState = LOW;
    
    int y,buzzerpin = 15;
    
    //Your Domain name with URL path or IP address with path
    String serverPath = "http://worldtimeapi.org/api/timezone/Europe/Istanbul";
    String serverPath1 = "192.168.43.136/earthquake";
    
    // the following variables are unsigned longs because the time, measured in
    // milliseconds, will quickly become a bigger number than can be stored in an int.
    unsigned long lastTime = 0;
    // Timer set to 10 minutes (600000)
    //unsigned long timerDelay = 600000;
    // Set timer to 5 seconds (5000)
    //unsigned long timerDelay = 5000;
    unsigned long timerDelay = 500; //edited
    
    int Gas_analog = A0;
    int Gas_digital = D0 ;
    
    float h,t;
    int gassensorAnalog, gassensorDigital;  //Analog kullanılmayacak. 
    
    void acilDurumBuzzer(){
      for(int u=0;u<5;u++){
    // Sounds the buzzer at the frequency relative to the note C in Hz
    tone(buzzerpin,261);    
    // Waits some time to turn off
    delay(200);
    //Turns the buzzer off
    noTone(buzzerpin); 
    // Sounds the buzzer at the frequency relative to the note D in Hz   
    tone(buzzerpin,293);             
    delay(200);    
    noTone(buzzerpin); 
    // Sounds the buzzer at the frequency relative to the note E in Hz
    tone(buzzerpin,329);      
    delay(200);
    noTone(buzzerpin);     
    // Sounds the buzzer at the frequency relative to the note F in Hz
    tone(buzzerpin,349);    
    delay(200);    
    noTone(buzzerpin); 
    // Sounds the buzzer at the frequency relative to the note G in Hz
    tone(buzzerpin,392);            
    delay(200);
    noTone(buzzerpin); 
      }
    }
        void alarmBuzzer(){
          for(int i=0;i<20;i++){
    // Sounds the buzzer at the frequency relative to the note C in Hz
    tone(buzzerpin,261);    
    // Waits some time to turn off
    delay(50);
    //Turns the buzzer off
    noTone(buzzerpin); 
    // Sounds the buzzer at the frequency relative to the note D in Hz   
    tone(buzzerpin,261);             
    delay(75);    
    noTone(buzzerpin); 
    // Sounds the buzzer at the frequency relative to the note E in Hz
    tone(buzzerpin,261);      
    delay(50);
    noTone(buzzerpin);     
    // Sounds the buzzer at the frequency relative to the note F in Hz
    tone(buzzerpin,261);    
    delay(75);    
    noTone(buzzerpin); 
    // Sounds the buzzer at the frequency relative to the note G in Hz
    tone(buzzerpin,261);            
    delay(50);
    noTone(buzzerpin);
    tone(buzzerpin,261);            
    delay(75);
    noTone(buzzerpin); 
     uzaklikSensoru();
      
    }
    String MesajKontrolAl(){
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        for (int i=0; i<numNewMessages; i++) {
        // Chat id of the requester
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id != CHAT_ID){
          bot.sendMessage(chat_id, "Unauthorized user", "");
          continue;
        }
        return bot.messages[i].text;
      }
    }

    
    // Handle what happens when you receive new messages
    void handleNewMessages(int numNewMessages) {
      /*Serial.println("handleNewMessages");
      Serial.println(String(numNewMessages));*/
    
      for (int i=0; i<numNewMessages; i++) {
        // Chat id of the requester
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id != CHAT_ID){
          bot.sendMessage(chat_id, "Unauthorized user", "");
          continue;
        }
        
        // Print the received message
        String text = bot.messages[i].text;
        Serial.println(text);
    
        String from_name = bot.messages[i].from_name;
    
        if (text == "/start") {
          String welcome = "Hoşgeldin, " + from_name + ".\n";
          welcome += "Aşağıda yazılan komutları kullanarak bana erişebilirsin.\n\n";
          welcome += "/nem \n";
          welcome += "/sicaklik \n";
          welcome += "/alarmekle \n";
          bot.sendMessage(chat_id, welcome, "");
        }
        if(text == "/alarmekle"){
          bot.sendMessage(chat_id, "Lütfen alarm saatinizi istenen biçimde giriniz. Örn:07:00", "");
          alarmSaat=MesajKontrolAl();
          if(alarmSaat=="/alarmekle"){bot.sendMessage(chat_id, "Alarm saati istenen süre içerisinde girilemedi.", ""); }
          else{bot.sendMessage(chat_id, "Alarm saatiniz başarıyla oluşturuldu. Alarm Saati:"+alarmSaat, "");uzaklikSayac=1; }
          }
        if (text == "/sicaklik") {
          bot.sendMessage(chat_id, "Odanin sicaklik degeri: " + String(t) + " C \n", "");
        }
        if (text == "/nem") {
          bot.sendMessage(chat_id, "Odanin nem degeri: "+ String(h), "");
        }
    
        if (text == "/led_on") {
          bot.sendMessage(chat_id, "LED state set to ON", "");
          ledState = HIGH;
          digitalWrite(ledPin, ledState);
        }
        
        if (text == "/led_off") {
          bot.sendMessage(chat_id, "LED state set to OFF", "");
          ledState = LOW;
          digitalWrite(ledPin, ledState);
        }
        
        if (text == "/state") {
          if (digitalRead(ledPin)){
            bot.sendMessage(chat_id, "LED is ON", "");
          }
          else{
            bot.sendMessage(chat_id, "LED is OFF", "");
          }
        }
      }
      }
    
    int uzaklikSensoru(){
      long duration;
      float distanceCm;
      float distanceInch;
        // Clears the trigPin
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      
      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH);
      
      // Calculate the distance
      distanceCm = duration * SOUND_VELOCITY/2;
      uzaklik=distanceCm;
      // Convert to inches
      distanceInch = distanceCm * CM_TO_INCH;
      if(uzaklik<30){uzaklikSayac=0;}
      // Prints the distance on the Serial Monitor
      Serial.print("uzaklık:");
      Serial.println(uzaklik);
      Serial.print("Distance (cm): ");
      Serial.println(distanceCm);
      Serial.print("Distance (inch): ");
      Serial.println(distanceInch);
      return distanceCm;
      }
      void alarmCal(String alarmSaat,String saat){
        Serial.println("alarm çal onksiyonu------------------Alarm Saati:"+alarmSaat+"Saat:"+saat);
        if(uzaklikSayac==1){
        if(saat==alarmSaat){
          alarmBuzzer();
          bot.sendMessage(CHAT_ID, "Alarm Çalıyor. Alarm Saati:"+alarmSaat, "");
          Serial.println("alarm çalıyor");
          ekranYazdir(saatApi(),nemSensor(), sicaklikSensor(), uzaklikSensoru());
          }
        }
        }
  
      void ekranYazdir(String saatDeger, int nemDeger, int sicaklikDeger, int uzaklikDeger){
         ekran.init();
         ekran.backlight();
         ekran.setCursor(0,0);
         ekran.print(saatDeger+ "  " + String(sicaklikDeger)+ "\337C "+" %" + String(nemDeger));
         ekran.setCursor(0,1);
         ekran.print("   "+String(tarih));
        }
        
       String saatApi(){
        String saat="Saat Bilgisi Alınamadı...";
                // Send an HTTP POST request depending on timerDelay--------------------------------------------
                
      if ((millis() - lastTime) > timerDelay) {
        //Check WiFi connection status
        if(WiFi.status()== WL_CONNECTED){
          WiFiClient client;
          HTTPClient http;
  
          // Your Domain name with URL path or IP address with path
          http.begin(client, serverPath);
          
          // Send HTTP GET request
          int httpResponseCode = http.GET();
          
          if (httpResponseCode>0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            //deserializeJson(tarihsaat, payload);
            //saatObje = tarihsaat["datetime"];
            //saat=String(saatObje);
            //String saatarih=payload["datetime"];
            deserializeJson(tarihsaat, payload);
            JsonObject obj = tarihsaat.as<JsonObject>();
            temp=String(obj[String("datetime")]);
            saat=String(temp[11])+String(temp[12])+String(temp[13])+String(temp[14])+String(temp[15]);
            tarih=String(temp[0])+String(temp[1])+String(temp[2])+String(temp[3])+String(temp[4])+String(temp[5])+String(temp[6])+String(temp[7])+String(temp[8])+String(temp[9]);
            Serial.println(temp);
            Serial.println(tarih);
            Serial.println(saat);
            //saat = String(payload[74])+String(payload[75])+String(payload[76])+String(payload[77])+String(payload[78]);
            //Serial.println(saat);
          }
          else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
          }
          // Free resources
          http.end();
        }
        else {
          Serial.println("WiFi Disconnected");
        }
        lastTime = millis();
      }
      return saat;
          }
  
    int gasSensor(){
      int gassensorDigital=digitalRead(D0);
      return gassensorDigital;
      }
  
    int sicaklikSensor(){
      h = dht.readHumidity();
      t = dht.readTemperature();
      float f = dht.readTemperature(true);
      Serial.print("Sicaklik=");
      Serial.print(t);
      Serial.println(" %\t");
      Serial.print("Nem=");
      Serial.println(h);
      Serial.print("Mq9 Digital Deger=");
      Serial.println(gassensorDigital);
      //delay(2000);
    
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        return 0;
      }
      return t;
      }
  
    int nemSensor(){
      h = dht.readHumidity();
      t = dht.readTemperature();
      float f = dht.readTemperature(true);
      Serial.print("Sicaklik=");
      Serial.print(t);
      Serial.println(" %\t");
      Serial.print("Nem=");
      Serial.println(h);
      Serial.print("Mq5 Digital Deger=");
      Serial.println(gassensorDigital);
      //delay(2000);
    
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        return 0;
      }
      return h;
      }

      void depremSensor(){
        //Check WiFi connection status
        if(WiFi.status()== WL_CONNECTED){
          WiFiClient client;
          HTTPClient http;
  
          // Your Domain name with URL path or IP address with path
          http.begin(client, "http://192.168.43.136/earthquake"); //192.168.43.136/earthquake
          
          // Send HTTP GET request
          int httpResponseCode = http.GET();
          
          if (httpResponseCode>0) {
            Serial.print("earthquake--HTTP Response code: ");
            Serial.println(httpResponseCode);
            String veri = http.getString();
            Serial.println(veri);
                    if (veri=="1"){
        bot.sendMessage(CHAT_ID, "Deprem oluyor!!", ""); //chat_id sorunu!!!
        int i=0;
        while(i<3){
        ekran.clear();
        ekran.setCursor(0,0);
        ekran.print("   Acil Durum");
        ekran.setCursor(0,1);
        ekran.print(" !Deprem Oluyor!");
        acilDurumBuzzer();i++;}
        Serial.println("acil durum deprem tg msj gönderildi");
        ekranYazdir(saatApi(),nemSensor(), sicaklikSensor(), uzaklikSensoru());
        }
          }
          else {
            Serial.print("earthquake--Error code: ");
            Serial.println(httpResponseCode);
          }
          // Free resources
          http.end();
        }
        else {
          Serial.println("earthquake--WiFi Disconnected");
        }
        }
  
  
    
    void setup(){
        Serial.begin(9600);
        pinMode(Gas_digital, INPUT);
        pinMode (buzzerpin,OUTPUT);
        pinMode(inA,OUTPUT); 
        pinMode(inB,OUTPUT); 
        dht.begin();
      #ifdef ESP8266
        configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
        client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
      #endif
      pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
      pinMode(echoPin, INPUT); // Sets the echoPin as an Input
      pinMode(ledPin, OUTPUT);
      digitalWrite(ledPin, ledState);
    
      
      // Connect to Wi-Fi
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      #ifdef ESP32
        client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
      #endif
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
      }
      // Print ESP32 Local IP Address
      Serial.println(WiFi.localIP());
    
    }

    
    void loop(){
      if (millis() > lastTimeBotRan + botRequestDelay)  {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
        while(numNewMessages) {
          Serial.println("got response");
          handleNewMessages(numNewMessages);
          numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
      }
  
      depremSensor();
        if (gasSensor()==0){
        bot.sendMessage(CHAT_ID, "Acil Durum, Yanıcı Gaz Algılandı!!!", ""); //chat_id sorunu!!!
        int i=0;
        while(i<3){
        analogWrite(inA, 150);
        ekran.clear();
        ekran.setCursor(0,0);
        ekran.print("   Acil Durum");
        ekran.setCursor(0,1);
        ekran.print("  !Yanici Gaz!");
        acilDurumBuzzer();
        i++;
        }
        Serial.println("acil durum tg msj gönderildi");
        ekranYazdir(saatApi(),nemSensor(), sicaklikSensor(), uzaklikSensoru());
        }
        analogWrite(inA, 0);
        /*digitalWrite(inA,LOW);
        digitalWrite(inB,LOW); */
                ekranYazdir(saatApi(),nemSensor(), sicaklikSensor(), uzaklikSensoru());
        alarmCal(alarmSaat,saatApi());
    }
