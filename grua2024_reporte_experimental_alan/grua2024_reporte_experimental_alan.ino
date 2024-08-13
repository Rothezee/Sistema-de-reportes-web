//mayo 2024 nuevos cambios en entradas ,los datos abajo en pines estan actualizados 
// julio.esta es la version mas actualizada del pago, y envia las variables en lugar del mensaje para reportar por wifi
// aca se modifico la parte del sim 800
//este skech es la evolucion de grua2023_reporte
//hay unas lineas comentadas en el loop que leian 
//se hicieron cambios en las entradas 12/12/23
//modificado 5 de mayo 2023 la pinza afloja gradualmentese programa coin display y barrera
 #include <EEPROM.h>
 #include <LiquidCrystal_I2C.h>

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

int triger = 9;//sensor ultrasonido
 int echo = 8;//sensor ultrasonido
 int DATO11 = 12; // salida pwm motores /reset sim 800 #
 int DATO7 = 7;//entrada de barrera  
 int DATO3 = A0;//entrada programacion 
 int DATO5 = 3;//salida lectura barrera # inhibir monedero o billetero
 int EPINZA = 4;//entrada señal pinza
 int SPINZA = 5;// salida pwm para pinza
 int DATO6 = A2;//boton programar +
 int DATO10 = A1;//boton programar -
 int DATO12 = 6;// entrada ring #
 int ECOIN = 2;//lectura de entrada de credito
 int X = 0;
 int RDISTANCIA = 0;//referencia de distancia
 int TIEMPO = 0;
 int TIEMPOAUX = 0;//variable auxiliar sistema de tiempo de la pinza
 int B = 0;
 int A = 0;
 int PAGO = 20;
 int BARRERA = 0;
 int CLEAR = 0;
 unsigned int COIN ;
 int BANK = 0;
 unsigned int BANKTIEMPO = 0;
 unsigned int CONTSALIDA ;
  unsigned int Y ;//no se usa actualmente, era para dividir entrada /salida
  int Z = 0;
  int distancia = 0;
  int tiempo = 0;
unsigned int CTIEMPO = 0;
int AUX = 0;// variable usada para evitar ruido en cierre de pinza
int CREDITO = 0;//variable usada para los creditos
int AUXCOIN = 0;
int AUX2COIN = 0;
int FUERZA = 50;
int INICIO = 0;
int BORRARCONTADORES = LOW;
unsigned int PJFIJO = 0;
unsigned int PPFIJO = 0;
int BARRERAAUX = 0;
float FUERZAAUX = 0;
float FUERZAV = 0;
int TIEMPOAUX1 = 0;
int FUERZAAUX2 = 0;
int GRUADISPLAY = 0;
int BARRERAAUX2 = 0;
char DAT;
int TIEMPO7 = 0;
long TIEMPO8 = 0;
char MENSAJE = "" ;
int TIEMPO9 = 0;
int AUXSIM = 0;
int TIEMPO5 = 0;
int prevPJFIJO = 0;
int prevPPFIJO = 0;
int prevBANK = 0;
int prevnose = 0;
int pesos = 0;
void setup() {
  // put your setup code here, to run once:
   TCCR2B = TCCR2B & B11111000 | B00000110;
   Serial.begin (9600);
  
   PAGO = 10 ;

  lcd.init();
  lcd.backlight(); 
   
   pinMode(triger,OUTPUT);
  pinMode(echo,INPUT); 
  pinMode (DATO7,INPUT_PULLUP);
  pinMode (DATO5,OUTPUT);
  pinMode (DATO3,INPUT_PULLUP);
  pinMode (SPINZA,OUTPUT);
  pinMode (EPINZA,INPUT_PULLUP);
  pinMode (DATO11,OUTPUT);
  pinMode (DATO6,INPUT_PULLUP);
  pinMode (DATO10,INPUT_PULLUP);
  pinMode (DATO12,INPUT_PULLUP);
  pinMode (ECOIN,INPUT_PULLUP);

  EEPROM.get(17,INICIO);
  if (INICIO != 35){EEPROM.put(1,0); EEPROM.put(3,0);EEPROM.put(5,0);
    EEPROM.put (7,12);EEPROM.put(9,2000);EEPROM.put(11,50);EEPROM.put(13,0);
    EEPROM.put (17,35); EEPROM.put(19,0);EEPROM.put(21,0);EEPROM.put(15,0);
    EEPROM.put(23,1000);}

  EEPROM.get(1,COIN);
  EEPROM.get(3,CONTSALIDA);
  EEPROM.get(5,BANK);
  EEPROM.get(7,PAGO);
  EEPROM.get(9,TIEMPO);
  EEPROM.get(11,FUERZA);
  EEPROM.get(13,PJFIJO);
  EEPROM.get(15,PPFIJO);
  EEPROM.get(19,BARRERAAUX2);
  EEPROM.get(21,GRUADISPLAY);
  EEPROM.get(23,TIEMPO5);
  
   if (digitalRead(DATO3) == LOW){ lcd.clear ();lcd.setCursor(0,0);lcd.print("CONT.FIJOS"); lcd.setCursor(0,1);lcd.print("PJ:"); 
     lcd.setCursor(3,1);lcd.print(PJFIJO); lcd.setCursor(9,1);lcd.print("PP:");
     lcd.setCursor(12,1);lcd.print(PPFIJO);delay (1000);
      while (digitalRead(DATO3) == HIGH) { delay (20);} }delay (1000);
   
  Y = 2;
    while(B <=10){    //hace una prueba con la barrera y determina la distancia para operar
   digitalWrite(triger,LOW);
  delayMicroseconds(2);
  digitalWrite(triger,HIGH);
  delayMicroseconds(10);
  tiempo = pulseIn(echo,HIGH);
  distancia = tiempo/10;
  RDISTANCIA = distancia ;
  B ++;
  delay(50);}

  digitalWrite (DATO11,1);// inicializa el dato 11 mantiene a 0 el reset del sim 800
  } 

//void leemensaje (){
  //Serial.flush();
  //Serial1.println("AT+CMGF=1");
  //serialCheck ();
  //Serial1.println ("AT+CNMI=2,2,0,0,0");
  //serialCheck ();
//}


//remplazar la variable nose por una valida
 void mensaje (){
String jsonMessage = "{\"device_id\":\"Maquina_1\",\"dato1\":" + String(PJFIJO) + ",\"dato2\":" + String(PJFIJO) + ",\"dato3\":" + String(PPFIJO) + ",\"dato4\":" + String(BANK) + "}\n";
  Serial.print(jsonMessage);
  }

void serialCheck (){

  // while (Serial.available()){
  // Serial1.write(Serial.read());}

   while (Serial.available()){
  Serial.write(Serial.read());
  }
  delay(200); }

 void leerbarrera (){
  
  if (BARRERAAUX2 == 1){
  digitalWrite(triger,LOW);
  delayMicroseconds(2);
  digitalWrite(triger,HIGH);
  delayMicroseconds(10);
  tiempo = pulseIn(echo,HIGH);
  distancia = tiempo/10;
   if ((distancia + 30) < RDISTANCIA){ BARRERA = HIGH;  }
   if ((distancia - 30) > RDISTANCIA){ BARRERA = HIGH;  }
   if (BARRERA == HIGH){ lcd.setCursor(0,1);lcd.print("###");delay(2500);graficar ();
   if (digitalRead (DATO12) == HIGH) {
     CONTSALIDA ++;PPFIJO ++; BANK = (BANK - PAGO);BARRERA = LOW; BARRERAAUX = HIGH;
     graficar(); EEPROM.put(3,CONTSALIDA);EEPROM.put(15,PPFIJO);EEPROM.put(5,BANK);} } }

   if (BARRERAAUX2 == 0){ 
     if (digitalRead (DATO7) == HIGH){BARRERA = HIGH;}
     if (BARRERA == HIGH){AUXSIM = HIGH; lcd.setCursor(0,1);lcd.print("###");delay(1000);
     CONTSALIDA ++;PPFIJO ++; BANK = (BANK - PAGO);BARRERA = LOW; BARRERAAUX = HIGH;
     graficar();  EEPROM.put(3,CONTSALIDA); EEPROM.put(15,PPFIJO);EEPROM.put(5,BANK);  } }
 }
     
    void  ajustebarrera (){
   
  digitalWrite(triger,LOW);
  delayMicroseconds(2);
  digitalWrite(triger,HIGH);
  delayMicroseconds(10);
  tiempo = pulseIn(echo,HIGH);
  distancia = tiempo/10;
   if ((distancia + 30) < RDISTANCIA){ BARRERA = HIGH;  }
   if ((distancia - 30) > RDISTANCIA){ BARRERA = HIGH;  }
   if (BARRERA == HIGH){ lcd.setCursor(0,1);lcd.print("###");delay(1000);}}

 
   void programar (){

     lcd.clear ();lcd.setCursor(0,0);lcd.print("VERSION 1.7");  lcd.setCursor(0,1);lcd.print("24/5/24");delay (500);
      while (digitalRead(DATO3) == HIGH) { delay (20);} 

    lcd.clear ();lcd.setCursor(0,0);lcd.print("PJ:");lcd.setCursor(4,0);lcd.print(PJFIJO);
    lcd.setCursor(0,1);lcd.print("PP:");lcd.setCursor(4,1);lcd.print(PPFIJO); delay (500);
    while (digitalRead(DATO3) == HIGH) { delay (20);}GRUADISPLAY = 0;

    graficar (); delay (500);
     while (digitalRead(DATO3) == HIGH) { delay (20);}EEPROM.get(21,GRUADISPLAY);
 
    lcd.clear ();lcd.setCursor(0,0);lcd.print("BORRA CONTADORES");  lcd.setCursor(0,1);lcd.print("NO");delay (500);
     while (digitalRead(DATO3) == HIGH){ 
   if (digitalRead(DATO6) == LOW){BORRARCONTADORES = HIGH;lcd.setCursor(0,1);lcd.print("SI"); delay (500);}
   if (digitalRead(DATO10) == LOW) {BORRARCONTADORES = LOW;lcd.setCursor(0,1);lcd.print("NO");  delay (500);} }
   if (BORRARCONTADORES == HIGH){ EEPROM.put(1,0); EEPROM.put(3,0);EEPROM.put(5,0); EEPROM.get(1,COIN); EEPROM.get(3,CONTSALIDA);EEPROM.get(5,BANK);
   BORRARCONTADORES = LOW ;lcd.clear ();lcd.setCursor(0,0);lcd.print("BORRADOS");delay (1000);  } 
    

     lcd.clear ();lcd.setCursor(0,0);lcd.print("Display Modo");
     if(GRUADISPLAY == 0){lcd.setCursor(0,1);lcd.print("Modo Contadores");}
     if(GRUADISPLAY == 1){lcd.setCursor(0,1);lcd.print("Modo Coin");}delay (500);
    while (digitalRead(DATO3) == HIGH) {
   if (digitalRead(DATO6) == LOW){GRUADISPLAY = 0;lcd.setCursor(0,1);lcd.print("Modo Contadores"); }
   if (digitalRead(DATO10) == LOW ) {GRUADISPLAY = 1 ;lcd.setCursor(0,1);lcd.print("Modo Coin"); }
   }EEPROM.put(21,GRUADISPLAY); 
     
    lcd.clear ();lcd.setCursor(0,0);lcd.print("AJUSTAR PAGO"); delay (500);
   while (digitalRead(DATO3) == HIGH){  lcd.setCursor(0,1);lcd.print(PAGO);delay (100);
   if (digitalRead(DATO6) == LOW){PAGO ++;lcd.setCursor(0,1);lcd.print(PAGO); delay (400);}
   if (digitalRead(DATO10) == LOW) {PAGO --;lcd.setCursor(0,1);lcd.print(PAGO);  delay (400);}
   } EEPROM.put (7,PAGO);
   
     lcd.clear ();lcd.setCursor(0,0);lcd.print("AJUSTAR TIEMPO"); delay (500);  
    while (digitalRead(DATO3) == HIGH) {  lcd.setCursor(0,1);lcd.print(TIEMPO);delay(100);
   if (digitalRead(DATO6) == LOW && TIEMPO < 5000){TIEMPO = (TIEMPO +10);lcd.setCursor(0,1);lcd.print(TIEMPO); }
   if (digitalRead(DATO10) == LOW && TIEMPO > 500) {TIEMPO = (TIEMPO -10);lcd.setCursor(0,1);lcd.print(TIEMPO); }
   } EEPROM.put (9,TIEMPO); 
    
     lcd.clear ();lcd.setCursor(0,0);lcd.print("TIEMPO F. FUERTE"); delay (500);  
    while (digitalRead(DATO3) == HIGH) {  lcd.setCursor(0,1);lcd.print(TIEMPO5);delay(100);
   if (digitalRead(DATO6) == LOW && TIEMPO5 < 5000){TIEMPO5 = (TIEMPO5 +10);lcd.setCursor(0,1);lcd.print(TIEMPO5); }
   if (digitalRead(DATO10) == LOW && TIEMPO5 > 0) {TIEMPO5 = (TIEMPO5 -10);lcd.setCursor(0,1);lcd.print(TIEMPO5); }
   } EEPROM.put (23,TIEMPO5); 

   
    lcd.clear ();lcd.setCursor(0,0);lcd.print("AJUSTAR FUERZA");delay (500);
    while (digitalRead(DATO3) == HIGH) { lcd.setCursor(0,1);lcd.print(FUERZA);delay(100);
   if (digitalRead(DATO6) == LOW && FUERZA < 101){FUERZA ++;lcd.setCursor(0,1);lcd.print(FUERZA); }
   if (digitalRead(DATO10) == LOW && FUERZA > 5) {FUERZA --;lcd.setCursor(0,1);lcd.print(FUERZA); }
   } EEPROM.put (11,FUERZA);
   
     lcd.clear ();lcd.setCursor(0,0);lcd.print("TIPO BARRERA");
     if(BARRERAAUX2 == 0){lcd.setCursor(0,1);lcd.print("INFRARROJO");}
     if(BARRERAAUX2 == 1){lcd.setCursor(0,1);lcd.print("ULTRASONIDO");}delay (500);
    while (digitalRead(DATO3) == HIGH) {
   if (digitalRead(DATO6) == LOW ){BARRERAAUX2 = 0;lcd.setCursor(0,1);lcd.print("INFRARROJO"); }
   if (digitalRead(DATO10) == LOW ) {BARRERAAUX2 = 1 ;lcd.setCursor(0,1);lcd.print("ULTRASONIDO"); }
   }EEPROM.put(19,BARRERAAUX2); 
   
     lcd.clear ();lcd.setCursor(0,0);lcd.print("PRUEBA BARRERA"); delay (500);
     while (digitalRead(DATO3) == HIGH) { lcd.setCursor(0,1);lcd.print(distancia);
    lcd.setCursor (8,1);lcd.print (RDISTANCIA); ajustebarrera ();
    if (BARRERA == HIGH){lcd.setCursor (0,1);lcd.print("###");delay(1000);BARRERA = LOW; }}
     
     graficar ();delay (500);
     
   }

   void graficar (){
    
    if (GRUADISPLAY == 0){
     lcd.clear();
     lcd.setCursor(0,0);lcd.print("PJ:"); 
     lcd.setCursor(3,0);lcd.print(COIN); 
     lcd.setCursor(9,0);lcd.print("PP:");
     lcd.setCursor(12,0);lcd.print(CONTSALIDA);
     lcd.setCursor(0,1);lcd.print("PA:");  
     lcd.setCursor(3,1);lcd.print(PAGO); 
     lcd.setCursor(7,1);lcd.print(BANKTIEMPO); 
     lcd.setCursor(9,1); lcd.print("BK:");
     lcd.setCursor(12,1);lcd.print(BANK);}
  
    if (GRUADISPLAY == 1){
     lcd.clear();
     lcd.setCursor(0,0);lcd.print("Credito");
     lcd.setCursor(8,0);lcd.print(CREDITO); }
   }

   void leecoin (){
    
     while (digitalRead(ECOIN) == LOW && AUXCOIN < 5){AUXCOIN ++;delay (1);
        if(digitalRead(ECOIN) == HIGH){AUXCOIN = 0;}}
     if (AUXCOIN == 5 && AUX2COIN == LOW){CREDITO ++;AUX2COIN = HIGH;graficar ();}
     while (digitalRead(ECOIN) == HIGH && AUXCOIN > 0){AUXCOIN --;delay (2);
        if(digitalRead(ECOIN) == LOW ){AUXCOIN = 5;}}
     if (AUXCOIN == 0 && AUX2COIN == HIGH){AUX2COIN = LOW;}
          
   } 

 
void loop() {


  Serial.print("dato1: "+ PJFIJO *100);
   graficar ();
  
   while(digitalRead ((EPINZA) == HIGH) && (AUX < 5)){CTIEMPO ++ ; // espera a que la señal de pinsa se ponga en high
     if (digitalRead (EPINZA) == LOW ) {AUX  ++ ;}
     if (digitalRead (EPINZA) == HIGH ) { AUX = 0;}

     if (TIEMPO7 < 20000){TIEMPO7 ++; delay (1);}
     if (TIEMPO7 == 19999){lcd.clear();lcd.setCursor(0,0); lcd.print("espere");mensaje (); delay (3000); graficar ();}

     if (digitalRead (DATO12) == 0){AUXSIM = HIGH; lcd.setCursor(0,0); lcd.print("espere");delay (5000);}
     if (digitalRead (DATO12) == 1 && AUXSIM == HIGH){mensaje ();delay (6000);AUXSIM = LOW;BARRERA = 0;
       graficar (); } //lee el estado del Ring del sim 800 para detectar llamada
     
     //TIEMPO9 ++;                  //lee los mensajes del sim,  tiempo9 regula cada cuanto tiempo lee el puerto del sim
     //if (TIEMPO9 == 30000){TIEMPO9 = 0;leemensaje ();
     //  if(Serial1.available ()){MENSAJE = Serial1.read (); delay (20);if (MENSAJE == '@' ){delay (100);TIEMPO7 = 10000 ; MENSAJE = 'X'; 
      // lcd.clear();lcd.setCursor(0,0); lcd.print("mensaje");delay (3000);             }}}
     
    leecoin ();

    if(Serial.available()){

      MENSAJE = Serial.read();

      if(PJFIJO != prevPJFIJO || PPFIJO != prevPPFIJO || BANK != prevBANK || PJFIJO * 100 != prevPJFIJO * 100){
        mensaje();
        prevPJFIJO = PJFIJO;
        prevPPFIJO = PPFIJO;
        prevBANK = BANK;
        pesos = PJFIJO * 100;

      }
    }
    if (( digitalRead (DATO3)) == LOW){  programar ();} //entra a la rutina de programar

    if(CTIEMPO >= 18000 && BANKTIEMPO > 0){  CTIEMPO = 0;BANKTIEMPO -- ;} //usa un contador de tiempo para saber si esta jugando seguido
   
    if (TIEMPO8 <200){TIEMPO8 ++;//regula cada cuanto tiempo se lee la barrera
    if (TIEMPO8 == 199){TIEMPO8 = 0;
    if (BARRERAAUX == LOW){leerbarrera ();}} }
    

     }

    BARRERAAUX = LOW; 
    AUX = 0;
    BARRERA = LOW;                  //detecto el cierre de pinza comienza a corrrer el programa
            

 if (CREDITO >= 1) {CREDITO -- ;}
 graficar ();
  
  COIN ++ ;
  BANK ++ ;
  PJFIJO ++;
  if( BANKTIEMPO < 10){ BANKTIEMPO ++ ;}
 
  EEPROM.put(5,BANK);
  EEPROM.put(13,PJFIJO);
  EEPROM.put(1,COIN);
 
  Z = random (5);
  
    if (PAGO <= BANK && Z <= 3 ){ analogWrite (SPINZA,250); delay (2000);// esta lina cierra la pinza cuando tiene que pagar
    
      while ((digitalRead(EPINZA) == HIGH) ||( X < 100)){ // este bloque es para amortiguar posibles aberturas temporales en la señal de pinsa
      if (digitalRead(EPINZA) == HIGH ){X = 0;}
      if ( X == 15 ){analogWrite (SPINZA,0);}
      if (digitalRead(EPINZA) == LOW){X ++ ;}

        if (TIEMPO8 <550){TIEMPO8 ++;//regula cada cuanto tiempo se lee la barrera
    if (TIEMPO8 == 549){TIEMPO8 = 0;
    if (BARRERAAUX == LOW){ leerbarrera (); }}}

      if (BARRERA == HIGH){break ;} }}
            
  else{
    analogWrite (SPINZA,255);delay (TIEMPO5);// este bloque se encarga del cierre de la pinsa cuando no esta para pagar

      FUERZAAUX2 = FUERZA;//asigna el valor a FUERSAAUX2
    if (BANK <= (-10)){FUERZAAUX2 = (FUERZA * 0.8);}//si BANK esta mas negativo que -10 disminuye la fuerza momentaneamente 20% 
      FUERZAV = random ((FUERZAAUX2*1.8),(FUERZAAUX2*2.5));
      FUERZAAUX = (FUERZAV - (FUERZAAUX2))/10; //acaafloja progresivamente la fuerza desde la fuerza maxima a hasta la ajustada en el parametro FUERZAAUX2 
      TIEMPOAUX = 0;
      TIEMPOAUX1 = (TIEMPO/10);
    while (TIEMPOAUX <= 10){FUERZAV = (FUERZAV - FUERZAAUX);
        analogWrite(SPINZA,(FUERZAV));
        TIEMPOAUX ++;
        delay (TIEMPOAUX1);}
    
    analogWrite (SPINZA,FUERZA*1.3); delay (300);
    analogWrite (SPINZA,FUERZA);delay (100);
    analogWrite (SPINZA,FUERZA*1.3); 
   
    while ((digitalRead(EPINZA) == HIGH) || (X < 100)){ // igual al anterior amortigua aberturas temporales
      if (digitalRead(EPINZA) == HIGH ){X = 0;}
      if ( X == 15 ){analogWrite (SPINZA,0);}
      if (digitalRead(EPINZA) == LOW){X ++ ;} 
        if (TIEMPO8 <550){TIEMPO8 ++;//regula cada cuanto tiempo se lee la barrera
    if (TIEMPO8 == 549){TIEMPO8 = 0;
    if (BARRERAAUX == LOW){ leerbarrera (); }}  }

      if (BARRERA == HIGH){break ;} } }
    
  analogWrite(SPINZA,0);
                         //termino el tiempo de la pinza cerrada y ahora se lee la barrera desde el primer bloque
  TIEMPO8 = 0;
    
  X = 0; 
  A = 0;
 
}
