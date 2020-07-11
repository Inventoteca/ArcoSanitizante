/*
 * Código para arco desinfectante para DD Biotech
 * Vigila la cantidad de agua en el deposito, ante la presencia dentro del arco
 * rocía agua por 20s, espera 5s y después vuelve a hacer lecturas de presencia.
*/

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h> 

//-------------Configuración LCD----------------------------------
#define I2C_ADDR    0x20 //Dirección del módulo de MJKDZ
#define  LED_OFF  0
#define  LED_ON  1
LiquidCrystal_I2C lcd(I2C_ADDR, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);
//-------------Fin Configuración LCD-------------------------------

//-------------Configuración Ultrasonico----------------------------------
const int pinecho = 11;
const int pintrigger = 12;
unsigned int tiempo, distancia;
//-------------Fin Configuración Ultrasonico------------------------------

//CALIBRACIONES
int distMin=17; //Calibrar esto de acuerdo al tamaño del tanque de agua con medidas en centímetros,
int distMax=120; //se puede obtener de la lectura del sensor en puerto serial
int porcentMin=10; //A partir de cuánta agua se desactiva el aspersor

int pinSensorIR=8; //En qué pin se conecta el sensor IR
int pinRele=5; //Para activar la bomba de agua

int lecturaIR = 0; //Inicializar variable.

void setup()  
{
  Serial.begin(9600);
  pinMode(pinecho, INPUT);
  pinMode(pintrigger, OUTPUT);
  pinMode(pinRele, OUTPUT);
  pinMode(pinSensorIR, INPUT_PULLUP);

  
  //pinMode(10, OUTPUT); //Comentar estos dos después de la prueba
  //digitalWrite(10, LOW);
  
  lcd.begin (20,4);  // Inicializar lcd 
  lcd.setBacklight(LED_ON);
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("DD BIOTECH V2.0"); 
  lcd.setCursor(0,1);
  lcd.print("INICIANDO..."); 
  delay(2000); 
  
  lcd.clear(); 
  lcd.setBacklight(LED_OFF);
  delay(300); 
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("     DD BIOTECH     "); 
}

//++++++++++++++++VOID LOOP++++++++++++++++++++
void loop()
{
  int x = PorcentajeDeposito();  
  ShowNivelTanque(x);
  if(x<porcentMin) ShowAdvertencia();
  else
  {
    ShowReady();
    lecturaIR = digitalRead(pinSensorIR);
    if(!lecturaIR) 
    {
      AspersorON(); //Activa aspersores por 20s
      delay(5000);
    }
  }
  delay(300);
}
//++++++++++++++++FIN VOID LOOP++++++++++++++++

float PorcentajeDeposito(void){
  
  //Medición ultrasónico
  digitalWrite(pintrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pintrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pintrigger, LOW);
  tiempo = pulseIn(pinecho, HIGH);
  distancia = tiempo*0.034/2; //Constante de la velocidad del sonido
  Serial.print(distancia); //Imprime la medición
  Serial.println(" cm");

  float rango = distMax - distMin;
  float porcentaje = 100 - (distancia - distMin)*(100/rango);
  return porcentaje;
  
}
void ShowNivelTanque(int niv){ //*CAMBIAR 20x4
  
  lcd.setCursor(0,1); //20*4
  //lcd.setCursor(0,0); //16*2
  lcd.print("   TANQUE AL "); 
  lcd.print(niv); 
  lcd.print("%   ");
  
  lcd.setCursor(0,2); //20*4
  //lcd.setCursor(0,1); //16*2
  if(niv>80 && niv<100)     lcd.print("      [|||||}   ");
  else if(niv>60 && niv<80) lcd.print("      [|||| }   ");
  else if(niv>40 && niv<60) lcd.print("      [|||  }   ");
  else if(niv>20 && niv<40) lcd.print("      [||   }   ");
  else if(niv>10 && niv<20) lcd.print("      [|    }   ");
  else if(niv<10 && niv>0)  lcd.print("      [  !  }   ");
  else                      lcd.print("      [ERROR}   ");
}
void ShowAdvertencia(void){
  
  lcd.setCursor(0,3); //20*4
  //lcd.setCursor(0,1); //16*2
  lcd.print("   DEPOSITO VACIO   "); 
  lcd.setBacklight(LED_OFF);
  delay(300); 
  lcd.backlight(); 
  
}

void ShowReady(void){
  
  lcd.setCursor(0,3); //20*4
  //lcd.setCursor(0,1); //16*2
  lcd.print("     ADELANTE!      "); 
  
}

void AspersorON(void){

  digitalWrite(pinRele,HIGH);
  lcd.setCursor(0,3); //20*4
  //lcd.setCursor(0,1); //16*2
  lcd.print("   DESINFECTANDO.   ");
  delay(20000);
  lcd.print("       LISTO.       ");
  digitalWrite(pinRele,LOW);
  
}
