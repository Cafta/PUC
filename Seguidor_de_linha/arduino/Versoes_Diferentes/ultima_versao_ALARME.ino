bool estado = false;
int temporizador = millis();
bool flag_alarme = true;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
}

void piscaLed(){
  estado = !estado;
  digitalWrite(LED_BUILTIN, estado);
  //delay(250);
}

void loop()
{
  if (flag_alarme == true){ // ALARME
    //lcd.setCursor(0, 0);
    //lcd.print("Objeto na via!");
    
    if(millis() - temporizador >= 250){
      temporizador = millis();
      piscaLed();
    }
  }
}