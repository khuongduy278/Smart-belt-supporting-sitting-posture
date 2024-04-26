FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void firebase_init(){
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Da ket noi voi firebase");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void sentDatatofirebase(int stt, int dataValue) {
    if (Firebase.ready() && signupOK) {
    char snum[8];
    itoa(stt, snum, 10);
    char sensorSent[100];
    strcpy(sensorSent, "sensor/");
    strcat(sensorSent, snum);
      if (Firebase.RTDB.setInt(&fbdo,sensorSent, dataValue)) 
      {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
}


void sentCountertofirebase(int stt) {
    if (Firebase.ready() && signupOK) {
      if (Firebase.RTDB.setInt(&fbdo,"/status/counter", stt)) 
      {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
}

void clearSensordata(){
  if (Firebase.RTDB.setInt(&fbdo,"/sensor", 0)) 
    {
      Serial.println("PASSED clear");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
}

int getData(char sensorJson[200]){
  if (Firebase.RTDB.getInt(&fbdo, sensorJson)) {
    if (fbdo.dataType() == "int") {
      int intValue = fbdo.intData();
      return intValue;
    }
  }
  else {
    Serial.println(fbdo.errorReason());
    return 0;
  }
}