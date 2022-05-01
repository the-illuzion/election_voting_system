#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

SoftwareSerial mySerial(14, 12);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id = 0;

const char *serverName = "http://ec2-3-108-60-8.ap-south-1.compute.amazonaws.com:3000/";
//const char *serverName = "http://192.168.120.116:3000/";

const char *ssid = "Pratibha";
const char *password = "mynordce";
const char *machineId = "001";

int but1 = 0;
int but2 = 13;
int but3 = 3;
int but4 = 1;
int mainBut = 2;

String state = "idle";
String vuid;

int enrollTry = 0;
int votingStep = 0;
String votingId;
String matchedId;
int finalVote;

int but1State = HIGH;
int but2State = HIGH;
int but3State = HIGH;
int but4State = HIGH;
int mainButState = HIGH;

String c1 = "c1";
String c2 = "c2";
String c3 = "c3";
String c4 = "c4";

void setup() {
  // put your setup code here, to run once:
  pinMode(but1, INPUT_PULLUP);
  pinMode(but2, INPUT_PULLUP);
  pinMode(but3, INPUT_PULLUP);
  pinMode(but4, INPUT_PULLUP);
  pinMode(mainBut, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Waiting to connect");
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  
  finger.begin(57600);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connected");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome");
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(enrollTry == 0 && votingStep == 0){
      checkStatus();
  }

  if(state ==  "\"idle\"" ){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome");
  }
  else if(state == "\"registering\""){
    if(enrollTry == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Registering");
      getVoterId();
    }
    else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Try Again");
    }

     delay(1000);
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Enrolling ID # ");
     lcd.print(id);

    while (!  getFingerprintEnroll() );
  }
  else if(state == "\"voting\""){
    if(votingStep == 0){
      getVotingDate();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Press Enter");
      votingStep += 1;
    }
    else if(votingStep == 1){
      mainButState = digitalRead(mainBut);
      if(mainButState == LOW){
        votingStep = 2;
      }
    }
    else if(votingStep == 2){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Get Ready");
      delay(2000);
      while (!getFingerprintID());
      delay(50);   
      votingStep = 3;
    }
    else if(votingStep == 3){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Get Ready");
      lcd.setCursor(0,1);
      lcd.print("Press Enter");
      votingStep = 4;
    }
    else if(votingStep == 4){
      mainButState = digitalRead(mainBut);
      if(mainButState == LOW){
        votingStep = 5;
      }
      
    }
    else if(votingStep == 5){
      lcd.clear();
      lcd.setCursor(0,0);
      getCandidateData();
      votingStep = 6;
    }
    else if(votingStep == 6){
      Serial.println("here");
      but1State = digitalRead(but1);
      Serial.println(but1State);
      if(but1State == LOW){
        votingStep = 7;
        finalVote = 1;
      }
      
      but2State = digitalRead(but2);
      Serial.println(but2State);
      if(but2State == LOW){
        votingStep = 7;
        finalVote = 2;
      }
    }
    else if(votingStep == 7){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Thanks");
      delay(2000);
      votingStep = 8;
    }
    else if(votingStep == 8){
      castVote(); 
    }
    
  }
}

void castVote(){
  String url = "machine/castVote";
  String payload = "{\"machineId\":\""+ String(machineId) +"\",\"vote\":\""+ String(finalVote) +"\",\"vuid\":"+ vuid +"}";
  String result = postRequest(url, payload);
  votingStep = 0;
  finalVote = -1;
  matchedId = -1;
  vuid = "";
}

void getCandidateData(){
  String url = "machine/getCandidateData";
  String payload = "{\"machineId\":\""+ String(machineId) +"\"}";
  String result = postRequest(url, payload);
  lcd.print(getKeyValue(result, "candidate1"));
  lcd.setCursor(0,1);
  lcd.print(getKeyValue(result, "candidate2"));
}

void getVotingDate(){
  String url = "machine/getVotingData";
  String payload = "{\"machineId\":\""+ String(machineId) +"\"}";
  String result = postRequest(url, payload);
  String vuid_val = getKeyValue(result, "vuid");
  vuid = vuid_val;
  String bio_val = getKeyValue(result, "biometricId");
  votingId = bio_val;
}

void checkStatus(){
  String url = "machine/status";
  String payload = "{\"machineId\":\""+ String(machineId) +"\"}";
  String result = postRequest(url, payload);
  String state_val = getKeyValue(result, "state");
  state = state_val;
  Serial.println(state);
  if(state ==  "\"idle\"" ){
    int count = getKeyValue(result, "total").toInt();
    id = count + 1;
    Serial.println(id);
  }
  Serial.println(vuid);
}

void getVoterId(){
  String payload = "{\"machineId\":\""+ String(machineId) +"\"}";
  String url = "machine/getVoterId";
  String result = postRequest(url, payload);
  String vuid_val = getKeyValue(result, "vuid");
  vuid = vuid_val;
  Serial.println("here");
  Serial.println(vuid);
}

void getRequest(String reqUrl)
{
  Serial.println(reqUrl);

  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client;
    HTTPClient http;

    String serverPath = serverName + reqUrl;

    http.begin(client, serverPath.c_str());

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
}


String postRequest(String reqUrl, String payload)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client;
    HTTPClient http;

    String serverPath = serverName + reqUrl;
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverPath);

    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(payload);

    String output = "{}";
    output = http.getString();
    if(httpResponseCode == 200){
      return output;
    }
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
}


String getKeyValue(String object, String key){
  Serial.println(object);
  JSONVar myObject = JSON.parse(object);
  JSONVar keys = myObject.keys();
  for (int i = 0; i < keys.length(); i++)
    {
      if(keys[i] == key){
        JSONVar value = myObject[keys[i]];
        //String res = value;
        return JSON.stringify(value);
      }
    }
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Waiting for finger");
  Serial.print("Waiting for valid finger to enroll as #"); 
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Image taken");
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Image converted");
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Remove finger");
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ID ");
  lcd.print(id);
  Serial.print("ID "); Serial.println(id);
  p = -1;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place same finger again");
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Image taken");
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Image converted");
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Creating model");
  Serial.print("Creating model for #");  Serial.println(id);
  enrollTry += 1;
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Prints matched!");
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Communication error");
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Fingerprints did not match");
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    updateRegisterStatus();
    Serial.println("Stored!");
    return 10;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}


void updateRegisterStatus(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Prints stored!");
  String payload = "{\"machineId\":\""+ String(machineId) +"\",\"biometricId\":\""+ String(id) +"\",\"vuid\":"+ vuid +"}";
  String url = "machine/updateRegisterStatus";
  postRequest(url, payload);
  enrollTry = 0;
  vuid = "";
}


uint8_t getFingerprintID() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place your finger");
  delay(2000);
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Image taken");
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Image converted");
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Found a print match!");
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Did not find a match");
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  matchedId = String(finger.fingerID);
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  Serial.println(finger.fingerID);
  if(String(finger.fingerID) == "No finger detected"){
    return -1;
  }
  else{
    return 10;
  }
  
}