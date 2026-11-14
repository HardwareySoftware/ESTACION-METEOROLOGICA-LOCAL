
/* 
                        PROGRAMA DEMO PARA ESTACION METEOROLOGICA 2026
   
   Programa que permite visualizar los datos en pantalla TFT utilizando OTRA Placa Arduino Uno

   NOTAS:
         0. Requiere la señal inalambrica de la placa Arduino Uno que recibe los datos
         1. Este programa solo es para placa Arduino Uno adicional con la pantalla TFT
         2. La conexion de la UART es por cable

  

  Codigos de identificacion para cada variable fisica:
  A = Temperatura
  B = Humedad relativa
  C = Calidad del aire (Indice COV+NOx)
  D = Altitud
  E = Presion Atmosferica
  F = Iluminacion
  G = Indice de rayos UV
  H = Velocidad del viento (Anemometro)
  I = Direccion del viento (Veleta)
  J = Caudal de Lluvia (Pluviometro)
*/

// LIbrerias para uso de pantalla tft con controlador SPDF5408 (tableta roja)
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>

// Canales ANALOGICOS de control pantalla TFT
#define LCD_RESET A4
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0


// Definicion de COLORES a mostrar en pantalla TFT
#define BLACK 0x0000
#define RED 0xF800
#define GREEN 0x07E0
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); // Define los canales de ARDUINO a utilizar pantalla TFT

// Variable para leer "dato" de la variable de lectura de Tx
float dato1;
int dato2, dato3, dato4, dato5; 
int dato6, dato7, dato8, dato9; 
int dato10;


void setup() {

  Serial.begin(9600);

  tft.reset();  // RESET de la pantalla TFT
  uint16_t identifier = tft.readID(); // Lee el controlador de la pantalla TFT
  tft.begin(identifier);  // Habilita el controlador para operar
  // Codigo de identificador del controlador= 0x9325 para SPDF5408/ILI9341
  tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)
  tft.setRotation(15);  // 15 0 45 coloca la imagen en sentido OPUESTO (giro de 180)

  // Imprime mensaje de inicio en pantalla TFT
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(30,30);
  tft.print("Estacion Meteorologica");
  tft.setCursor(20,50);
  tft.print("-----------------------");
  tft.setCursor(250,70);
  tft.print("2026");

  tft.setTextColor(GREEN);
  tft.setTextSize(3);
  tft.setCursor(80,100);
  tft.print("Stand");
  tft.setCursor(150,140); // (Columna, fila)
  tft.print("by...");

  tft.setTextColor(RED);
  tft.setCursor(160,210); // Escribe un texto en la esquina SUPERIOR derecha de la pantalla tft
  tft.setTextSize(2);
  tft.print("Tec San Juan");

}

void mensaje(){

  // Texto en la esquina SUPERIOR derecha
  tft.setTextColor(RED);
  tft.setCursor(160,5); // Escribe un texto en la esquina SUPERIOR derecha de la pantalla tft
  tft.setTextSize(2);
  tft.print("Tec San Juan");

// Texto en la esquina INFERIOR derecha
  tft.setTextColor(RED);
  tft.setCursor(280,220); // Escribe un texto en la esquina SUPERIOR derecha de la pantalla tft
  tft.setTextSize(2);
  tft.print("Jg");

}

void loop() {

  String tmp= " "; // Servira de buffer temporal para los datos que llegan del inalambrico
  
  if (Serial.available() > 0){ // Si llego informacion al inalambrico la guardamos en tmp e imprimimos
    tmp = Serial.readStringUntil('#'); // Hasta que recibe un # lee el dato 
  }

// ****************************************************************************
//TEMPERATURA 

  if (tmp.charAt(0) == 'A'){ // si la cadena tiene A guarda el valor de A
    dato1 = tmp.substring(1).toFloat();  // Convierte la cadena recibida en un numero decimal

 
// IMPRESION DE TEMPERATURA EN PANTALLA TFT

// Limpia la pantalla para mostrar nuevo mensaje
  tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

  mensaje(); // Rutina para imprimir mesaje comun a todas las lecturas en pantalla TFT

// Imprime mensaje del nombre de la variable a mostrar
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(30,40);
  tft.print("Temperatura");

// Se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE);
  tft.setTextSize(7);
  tft.setCursor(60,100);
  tft.print(dato1, 1);  //   Muestra el valor recibido dato1
  tft.setTextSize(4);
  tft.print(" oC"); 
    
  if(dato1>= 33) { // Condicion para temperatura ALTA
    tft.setTextSize(3);
    tft.setCursor(90,180);
    tft.print("CALOR");
    } 
    else if(dato1>= 18 && dato1< 33) { // Condicion para temperatura AGRADABLE
      tft.setTextSize(3);
      tft.setCursor(80,180);
      tft.print("Agradable");
      } 
      else if(dato1>= 0 && dato1< 18 ) { // Condicion para temperatura BAJA
        tft.setTextSize(3);
        tft.setCursor(90,180);
        tft.print("FRIO"); 
      } 

      dato1 = 0; // Limpia el valor de la variable

  }

// ******************************************************************************
// HUMEDAD RELATIVA

  if (tmp.charAt(0) == 'B'){ // si la cadena tiene B guarda el valor de B
    dato2 = tmp.substring(1).toInt();  

// IMPRESION DE HUMEDAD EN PANTALLA TFT

// Limpia la pantalla para mostrar nuevo mensaje
  tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

// Imprime mensaje comun a todas las lecturas
  mensaje();

  // Imprime mensaje del nombre de la variable a mostrar
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(80,40);
  tft.print("Humedad");

  // Ahora se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE);
  tft.setTextSize(7);
  tft.setCursor(100,100);
  tft.print(dato2); 
  tft.setTextSize(4);
  tft.print(" %");
        
  if(dato2>= 60) {  
  tft.setTextSize(3);
  tft.setCursor(90,180);
  tft.print("Dia HUMEDO");
  } 
  else if(dato2>= 0 && dato2< 60) { 
    tft.setTextSize(3);
    tft.setCursor(90,180);
    tft.print("Dia SECO");
  }

    dato2= 0; 

  }

// ******************************************************************************
// CALIDAD DEL AIRE  

// Indice VOC+NOx UNIFICADO
  if (tmp.charAt(0) == 'C'){ // si la cadena tiene C guarda el valor de C
    dato3 = tmp.substring(1).toInt();  
  
// IMPRESION DE CALIDAD DEL AIRE EN PANTALLA TFT

// Limpia la pantalla para mostrar nuevo mensaje
  tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

// Imprime mensaje comun a todas las pantallas
  mensaje();

// Imprime mensaje del nombre de la variable a mostrar
  tft.setTextColor(GREEN);
  tft.setTextSize(3);
  tft.setCursor(20,40);
  tft.print("Calidad del aire");

// Se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE);
  tft.setTextSize(7);
  tft.setCursor(60,100);
  tft.print(dato3); 
  tft.setTextSize(4); 
  tft.print(" VOC");

  if(dato3 <= 100.0) {  
    tft.setTextSize(3);
    tft.setCursor(70,190);
    tft.print("Aire LIMPIO");
    } 
    else if (dato3 <= 300.0) {
      tft.setTextSize(3);
      tft.setCursor(70,190);
      tft.print("Aire REGULAR");
    } 
    else {
      tft.setTextSize(3);
      tft.setCursor(70,190);
      tft.print("Aire MALO");
    }

    dato3= 0; 

  }


// ******************************************************************************
// ALTITUD  

  if (tmp.charAt(0) == 'D'){ // si la cadena tiene D guarda el valor de D
    dato4 = tmp.substring(1).toInt();

// IMPRESION DE ALTITUD EN PANTALLA TFT

// Limpia la pantalla para mostrar nuevo mensaje
  tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

// Imprime mensaje comun para todas las lecturas en pantalla TFT
  mensaje();

// Imprime mensaje del nombre de la variable a mostrar 
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(60,40);
  tft.print("Altitud");

// Se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE);
  tft.setTextSize(7);
  tft.setCursor(50,100);
  tft.print(dato4);
  tft.setTextSize(4);
  tft.print(" m");
  tft.setTextSize(3);
  tft.setCursor(100,180);
  tft.print("s.n.m.");

  dato4= 0;

  } 

// ******************************************************************************
// IMPRESION DE PRESION ATMOSFERICA  

  if (tmp.charAt(0) == 'E'){ // si la cadena tiene E guarda el valor de E
    dato5 = tmp.substring(1).toInt();


// Limpia la pantalla para mostrar nuevo mensaje
  tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

// Imprime mensaje comun para todas las lecturas en la pantalla TFT
  mensaje();

// Imprime mensaje del nombre de la variable a mostrar  
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(70,40);
  tft.print("Presion");

// Se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE);  
  tft.setTextSize(7);
  tft.setCursor(50,100);
  tft.print(dato5);
  tft.setTextSize(4);
  tft.print(" hPa");
  tft.setTextColor(GREEN);
  tft.setTextSize(3);
  tft.setCursor(60,180);
  tft.print("Atmosferica");

  dato5= 0;

  }

// ******************************************************************************
// IMPRESION DE CANTIDAD DE ILUMINACION EN LUX

  if (tmp.charAt(0) == 'F'){ // si la cadena tiene F guarda el valor de F
    dato6 = tmp.substring(1).toInt();


// Limpia la pantalla para mostrar nuevo mensaje
  tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

// Imprime mensaje comun para todas las lecturas de pantalla TFT
  mensaje();

// Imprime mensaje del nombre de la variable a mostrar  
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(30,40);
  tft.print("Iluminacion");

// Se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE);  
  tft.setTextSize(7);
  tft.setCursor(40,100);
  tft.print(dato6);
  tft.setTextSize(4);
  tft.print(" lux");


  if(dato6<= 100 && dato6>= 95) { // Condicion para dia SOLEADO
    tft.setTextSize(3);
    tft.setCursor(60,180);
    tft.print("Dia SOLEADO");
    } 
    
    else if(dato6< 95 && dato6 >= 40) { // Condicion para dia DESPEJADO
      tft.setTextSize(3);
      tft.setCursor(120,180);
      tft.print("Dia");
      } 
      /*    NOTA: NO es posible determinar dia nublado con solo el valor de iluminacion (se requiere una variable mas que no tengo)
      else if(dato6< 75 && dato6>= 40) { // Condicion para dia NUBLADO 
        tft.setTextSize(3);
        tft.setCursor(60,180);
        tft.print("Dia NUBLADO"); 
      } 
      */
      else if(dato6< 40 && dato6>= 0) { // Condicion para NOCHE
        tft.setTextSize(3);
        tft.setCursor(90,180);
        tft.print("NOCHE"); 
      }

  dato6= 0;

  }

// *****************************************************************************
// IMPRESION DE INDICE UV

   if (tmp.charAt(0) == 'G'){ // si la cadena tiene G guarda el valor de G
    dato7 = tmp.substring(1).toInt();

// Limpia la pantalla para mostrar nuevo mensaje
   tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

// Imprime mensaje comun para todas las lecturas de pantalla TFT
  mensaje();

  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(60,50);
  tft.print("Indice UV");

// Se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE); 
  tft.setTextSize(7);
  tft.setCursor(140,100);
  tft.print(dato7);
  

//==================================================
// CLASIFICACION PROFESIONAL DEL INDICE UV
//==================================================

if (dato7 < 3) {

    tft.setTextSize(3);
    tft.setCursor(70,180);
    tft.print("Riesgo BAJO");

  } else if (dato7 < 6) {

    tft.setTextSize(3);
    tft.setCursor(50,180);
    tft.print("Riesgo MODERADO");

  } else if (dato7 < 8) {

    tft.setTextColor(YELLOW);
    tft.setTextSize(3);
    tft.setCursor(70,180);
    tft.print("Riesgo ALTO");

  } else if (dato7 < 11) {

    tft.setTextColor(RED);
    tft.setTextSize(3);
    tft.setCursor(50,180);
    tft.print("Riesgo MUY ALTO");

  } else {

    tft.setTextColor(RED);
    tft.setTextSize(3);
    tft.setCursor(50,180);
    tft.print("Riesgo EXTREMO");
  }

  dato7= 0;

   }

 

// ******************************************************************************
// IMPRESION DE VELOCIDAD DEL VIENTO  

  if (tmp.charAt(0) == 'H'){ // si la cadena tiene G guarda el valor de G
    dato8 = tmp.substring(1).toInt();

// Limpia la pantalla para mostrar nuevo mensaje
   tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

// Imprime mensaje comun para todas las lecturas de pantalla TFT
  mensaje();

  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(30,40);
  tft.print("Vel. Viento");

// Se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE); 
  tft.setTextSize(7);
  tft.setCursor(70,100);
  tft.print(dato8);
  tft.setTextSize(4);
  tft.print(" Km/h");

  if(dato8 >= 40) { // Condicion para Velocidad del viento ALTA
    tft.setTextSize(3);
    tft.setCursor(80,180);
    tft.print("FUERTE");
      } 
      else if(dato8 >= 20 && dato8 < 40) { // Condicion para velocidad MODERADA
      tft.setTextSize(3);
      tft.setCursor(80,180);
      tft.print("Moderado");
      } 
      else if(dato8 >= 0 && dato8 < 20 ) { // Condicion para velocidad LIGERA
        tft.setTextSize(3);
        tft.setCursor(100,180);
        tft.print("Ligero"); 
      } 


  dato8= 0;

  }


// ******************************************************************************
// IMPRESION DE DIRECCION DEL VIENTO  

  if (tmp.charAt(0) == 'I'){ // si la cadena tiene H guarda el valor de H
    dato9 = tmp.substring(1).toInt();

// Limpia la pantalla para mostrar nuevo mensaje
  tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

// Imprime mensaje comun para todas las lecturas de pantalla TFT
  mensaje();

  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(30,40);
  tft.print("Dir. Viento");

// Se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE); 
  tft.setTextSize(7);
  tft.setCursor(70,100);
  tft.print(dato9);
  tft.setTextSize(4);
  tft.print(" Gds");

  if(dato9 == 0) { // Valor para Direccion del viento hacia el NORTE
  tft.setTextSize(3);
  tft.setCursor(100,180);
  tft.print("Norte");
  } 

  if(dato9 == 45) { // Valor para Direccion del viento NORESTE
  tft.setTextSize(3);
  tft.setCursor(80,180);
  tft.print("NorEste");
  } 

  if(dato9 == 90) { // Valor para Direccion del viento ESTE
  tft.setTextSize(3);
  tft.setCursor(100,180);
  tft.print("Este");
  } 

  if(dato9 == 135) { // Valor para Direccion del viento SURESTE
  tft.setTextSize(3);
  tft.setCursor(80,180);
  tft.print("SurEste");
  } 

  if(dato9 == 180) { // Valor para Direccion del viento SUR
  tft.setTextSize(3);
  tft.setCursor(110,180);
  tft.print("Sur"); 
  } 

  if(dato9 == 225) { // Valor para Direccion del viento SUROESTE
  tft.setTextSize(3);
  tft.setCursor(50,180);
  tft.print("SurOeste");
  } 

  if(dato9 == 270) { // Valor para Direccion del viento OESTE
  tft.setTextSize(3);
  tft.setCursor(100,180);
  tft.print("Oeste"); 
  }

  if(dato9 == 315) { // Valor para DIreccion del viento NOROESTE
  tft.setTextSize(3);
  tft.setCursor(50,180);
  tft.print("NorOeste");
  } 

  dato9= 0;

  }


// ******************************************************************************
// IMPRESION DE PLUVIOMETRO  

  if (tmp.charAt(0) == 'J'){ // si la cadena tiene I guarda el valor de I
    dato10 = tmp.substring(1).toInt();

// Limpia la pantalla para mostrar nuevo mensaje
  tft.fillScreen(BLACK); // Color de pantalla de fondo (NEGRO)

// Imprime mensaje comun para todas las lecturas de pantalla TFT
  mensaje();

  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(80,40);
  tft.print("lluvia");

  // Se imprime el valor del dato ACTUALIZADO
  tft.setTextColor(WHITE); 
  tft.setTextSize(7);
  tft.setCursor(70,100);
  tft.print(dato10);
  tft.setTextSize(4);
  tft.print(" mm");


if(dato10 >= 60) { // Condicion para lluvia INTENSA
  tft.setTextSize(3);
  tft.setCursor(70,180);
  tft.print("INTENSA");
  } 
  else if(dato10 >= 30 && dato10 < 60) { // Condicion para lluvia MODERADA
    tft.setTextSize(3);
    tft.setCursor(60,180);
    tft.print("Moderada");
    } 
    else if(dato10 >= 10 && dato10 < 30 ) { // Condicion para lluvia LIGERA
    tft.setTextSize(3);
    tft.setCursor(80,180);
    tft.print("Ligera"); 
    } 
    else if(dato10 < 10){
      tft.setTextSize(3);
      tft.setCursor(70,180);
      tft.print("No lluvia");
    }

  dato10= 0;

  }

}







