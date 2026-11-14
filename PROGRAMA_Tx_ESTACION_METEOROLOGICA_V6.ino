
/*
**************************************************************************************
                 PROGRAMA ESTACION METEOROLOGICA "Tec San Juan"
                                                         (2026)

Programador: 
            Dr. Juan Gabriel Rodriguez Ortiz     (Con ayuda de CHATGPT)

                    Monitoreo de variables meteorologicas

Codigos de identificacion para cada variable fisica:
A = Temperatura            (AHT20)
B = Humedad relativa       (AHT20)
C = Calidad del aire       (SGP41)
D = Altitud                (BMP180)
E = Presion atmosferica    (BMP180)
F = Iluminacion            (LTR390)
G = Indice de rayos UV     (LTR390)
H = Velocidad del Viento   (ANEMOMETRO)
I = Direccion del Viento   (VELETA)
J = Cantidad de lluvia     (PLUVIOMETRO)

D4= led externo indicador de Tx

**************************************************************************************

*/

//===================================================
// LIBRERIAS
//===================================================

#include <Arduino.h>
#include <math.h>  // Permite realizar operaciones matematicas complejas
#include <Wire.h>  // Uso del protocolo I2C (Sensor AHT20,BMP180,LTR390 Y SGP41)
#include <Adafruit_AHTX0.h>
#include <Adafruit_LTR390.h>
#include <Adafruit_BMP085.h>
#include <SensirionI2CSgp41.h>
#include <NOxGasIndexAlgorithm.h>
#include <VOCGasIndexAlgorithm.h> // Header corregido con VOC en mayúsculas


//====================================================
// OBJETOS
//====================================================

Adafruit_AHTX0 aht;
Adafruit_LTR390 ltr = Adafruit_LTR390();
Adafruit_BMP085 bmp;
SensirionI2CSgp41 sgp41;
VOCGasIndexAlgorithm voc_algorithm; // Clase corregida con VOC en mayúsculas
NOxGasIndexAlgorithm nox_algorithm;


//====================================================
// VARIABLES
//====================================================

// SENSOR AHT20
unsigned long tiempoAnterior = 0;
const unsigned long intervalo = 1000; // 1 segundo

// SENSOR BMP180
float presion = 0.0;
float altitud = 0.0;
const float PRESION_NIVEL_MAR = 101325.0; // Presion promedio a nivel del mar

// SENSOR LTR390
float uvBase = 0.0;
float uvCorregido = 0.0;
int lux;
int uvi;

// SENSOR SGP41

// Variables para lecturas brutas
uint16_t rawVoc = 0;
uint16_t rawNox = 0;

// Variables para índices calculados
int32_t vocIndex = 0;
int32_t noxIndex = 0;

// Parámetros de humedad y temperatura por defecto
uint16_t defaultCompensationRh = 0x8000; // ~50% HR
uint16_t defaultCompensationT  = 0x6666; // ~25°C


//  ANEMOMETRO
int sensorPin= 2; // Pin para interrupcion lectura del Anemometro
int recordtime= 3;  // Variable que define el tiempo de mdicion en segundos
int Interruptcounter;  // Contador de interrupciones en D2
float velocidad;

// VELETA
int Pinsensor = A6;  // Pin para la lectura del POTENCIOMETRO
int vel= 0;  // Variable para guardar el dato recibido
int direccion;  // Variable par almacenar direccion del viento en grados

// PLUVIOMETRO
const int pluvio= 2;  // No. de pin donde se conecta el sensor de lluvia (D2)
const int umbraltiempo= 300;
volatile int ISRcontador= 0;
int contador= 0;
float litros= 0;
long tiempocontador= 0;
long tiempohora= 0;

// MISCELANEAS
const int pinled= 4; // Pin a utilizar para el led EXTERNO Tx (D4)
int tiempo= 2000; // Tiempo para cada transicion en pantalla TFT del Rx

//  ----------  FUNCIONES  -------------

// Calidad del aire (Clasificacion)
String obtenerClasificacion(int32_t valorIndice) {
  if (valorIndice <= 100) return "EXCELENTE (Limpio)";
  if (valorIndice <= 200) return "BUENA (Aceptable)";
  if (valorIndice <= 300) return "MODERADA (Media)";
  if (valorIndice <= 400) return "MALA (Alta)";
  return "MUY MALA (Critica)";
}

//  Anemometro
void countup() {
  Interruptcounter++;
}

//  Pluviometro
void contadorLitros() {
  if (millis()>(tiempocontador+umbraltiempo)) {
    ISRcontador++;
    tiempocontador=millis();
  }
}

void contarLluvia() {
  unsigned long tiempoInterrupcionActual = millis();
  
}


// Destello LED EXTERNO de Tx 

void Led() {
  digitalWrite(pinled, HIGH);
  delay(50);
  digitalWrite(pinled, LOW);
}



void setup() {

  Serial.begin(9600);  // A velocidad mayor NO hay comunicacion UART
  Wire.begin();  // Inicializa el bus I2C
  Wire.setClock(100000);  // Velocidad estándar para mejorar compatibilidad
  sgp41.begin(Wire);

  // Rutina para verificar estado del sensor SGP41
  uint16_t error;

  Serial.println("====================================");
  Serial.println("=     ESTACION METEOROLOGICA       =");
  Serial.println("=         Tec San Juan        2026 =");
  Serial.println("====================================");
  Serial.println();

  pinMode(pinled, OUTPUT); // Pin D4 como salida para el led EXTERNO
  pinMode(sensorPin, INPUT_PULLUP); // Se habilita la resistencia interna del canal 2 (D2) para el Anemometro
  pinMode(pluvio, INPUT_PULLUP);  // Se habilita la resistencia interna del canal 3 (D3) para el Pluviometro
  attachInterrupt(digitalPinToInterrupt(pluvio), contadorLitros, FALLING);  // Configura la interrupción en el pin físico (dispara cuando la señal CAE a LOW)

    // Verificar sensores
  Serial.println("Verificando sensores digitales...");
  Serial.println();
  delay(3000);

  // Inicializar AHT20
  Serial.println("--- Inicializando SENSOR AHT20 ---");

  if (!aht.begin()) {
    Serial.println("ERROR: No se detecta el sensor AHT20");
    while (1) {
      delay(100);
    }
  }

  Serial.println("AHT20 detectado correctamente");
  delay(2000);


  // Inicializar SGP41"
  Serial.println("--- Inicializando SENSOR SGP41 ---");

  uint16_t srawVoc;
  for (int i = 0; i < 10; i++) {
    sgp41.executeConditioning(defaultCompensationRh, defaultCompensationT, srawVoc);
    delay(1000);
  }

  Serial.println("SGP41 detectado correctamente");
  delay(2000);


  // Inicializar LTR390
  Serial.println("--- Inicializando SENSOR LTR390 ---");

  if (!ltr.begin()) {
    Serial.println("ERROR: Sensor LTR390 no detectado");
    while (1); {
      delay(100);
    }
  }

  Serial.println("LTR390 detectado correctamente");
  delay(2000);

  // Inicializar BMP180
  Serial.println("--- Inicializando SENSOR BMP180 ---");

  if (!bmp.begin()) {
    Serial.println("ERROR: Sensor BMP180 no detectado");
    while (1); {
      delay(100);
    }
  }

  Serial.println("BPM180 detectado correctamente");
  Serial.println("----------------------------------");
  delay(2000);

  // Configuracion de ganancia 1X y 16 bits para exterior / sol directo
  ltr.setGain(LTR390_GAIN_1);
  ltr.setResolution(LTR390_RESOLUTION_16BIT);

  }



void loop() {

  //=======================================================
  // PROCESAMIENTO DE DATOS PARA ENVIO DE MEDICIONES A Rx
  //=======================================================

  // Mensaje inicial a las mediciones
  Serial.print("Mediciones: ");
  Serial.println();
  delay(tiempo);

  //===============================================
  // LECTURA AHT20

  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervalo) {

    tiempoAnterior = tiempoActual;

    sensors_event_t humidity, temp;

    // La librería realiza internamente la sincronización
    aht.getEvent(&humidity, &temp);

    // Envio del dato TEMPERATURA a Rx
    Serial.print('A');
    Serial.print(temp.temperature, 1);
    Serial.println('#');
    Led();

    delay(tiempo);

    // Envio del dato HUMEDAD a Rx
    Serial.print('B');
    Serial.print(humidity.relative_humidity, 0); // Se requiere el 0 para eliminar el decimal
    Serial.println('#');
    Led();

    delay(tiempo);

  } 
  
  //===============================================
  // LECTURA CALIDAD DEL AIRE

  uint16_t error;

  // El algoritmo requiere un muestreo estricto de 1 Hz (1 vez por segundo)
  delay(1000);

  error = sgp41.measureRawSignals(defaultCompensationRh, defaultCompensationT, rawVoc, rawNox);

  // Procesar lecturas
  vocIndex = voc_algorithm.process(rawVoc);
  noxIndex = nox_algorithm.process(rawNox);

  // Determinar el valor único para la escala de 1 a 500
  int32_t indiceUnico = max(vocIndex, noxIndex);


  // Envio del dato CALIDAD DEL AIRE a Rx
  Serial.print('C');
  Serial.print(indiceUnico);  // VALOR DE INDICE VOC+NOx
  Serial.println('#');
  Led();

  delay(tiempo);


  //================================================
  // LECTURA BMP180

  //temperatura = bmp.readTemperature();
  presion = bmp.readPressure();
  altitud = bmp.readAltitude(PRESION_NIVEL_MAR);

  // Envio del dato ALTITUD a Rx
  Serial.print('D');
  Serial.print(altitud, 0);
  Serial.println('#');
  Led();

  delay(tiempo);

  // Envio del dato PRESION ATMOSFERICA a Rx
  Serial.print('E');
  Serial.print((presion / 100.0), 0);
  Serial.println('#');
  Led();

  delay(tiempo);

    
  // -------------------------------------------------------------
  // 1. MEDIDA Y CÁLCULO DE LUZ AMBIENTAL (LUX)
  // -------------------------------------------------------------
  ltr.setMode(LTR390_MODE_ALS);
  delay(150);

  if (ltr.newDataAvailable()) {
    uint32_t als_raw = ltr.readALS();
    
    // Formula de Lux segun datasheet de LTR390:
    // Lux = (0.6 * raw) / (Gain * Resolution_Factor)
    // Para Ganancia 1X factor_gain = 1; Para 16-bit factor_res = 1.0 (en 18-bit es 4.0, 20-bit es 16.0)
    lux = (0.6 * (float)als_raw) / 1.0;

  }

  // Envio del dato ILUMINACION a Rx
  Serial.print('F');
  Serial.print(lux);
  Serial.println('#');
  Led();

  delay(tiempo);


  // -------------------------------------------------------------
  // 2. MEDIDA Y CÁLCULO DE UVI (INDICE UV)
  // -------------------------------------------------------------
  ltr.setMode(LTR390_MODE_UVS);
  delay(150);

  if (ltr.newDataAvailable()) {
    uint32_t uvs_raw = ltr.readUVS();
    
    // Formula de UVI segun datasheet (Factor de sensibilidad de fabrica ~2300)
    uvi = (int)uvs_raw / 2300.0;

  }

  // Envio del dato INDICE RAYOS UV a Rx
  Serial.print('G');
  Serial.print(uvi);
  Serial.println('#');
  Led();

  delay(tiempo);

  //================================================
  // ANEMOMETRO

  Interruptcounter=0;
  attachInterrupt(digitalPinToInterrupt(sensorPin), countup, RISING);
  delay(1000 * recordtime);
  detachInterrupt(digitalPinToInterrupt(sensorPin));
  velocidad= (float)Interruptcounter / (float)recordtime * 2.4;

  // Envio del dato VELOCIDAD VIENTO a Rx
  Serial.print('H');
  Serial.print(velocidad, 0);
  Serial.println('#');
  Led();

  delay(tiempo);


  //================================================
  // VELETA

  vel = analogRead(Pinsensor);

  // Se definen solo 8 direcciones para el viento
  // (0, 45, 90, 135, 180, 225, 270 y 315 grados)

  if((vel>745) && (vel<824))
  //Serial.println("NORTE o 0 GRADOS");  // 0 grados 
  direccion= 0; 

  if((437<vel) && (vel<483))
  //Serial.println("NORESTE o 45");  // 45 grados
  direccion= 45; 

  if((vel>88) && (vel<97))
  //Serial.println("ESTE o 90 GRADOS");  // 90 grados
  direccion= 90;

  if((175<vel) && (vel<193))
  //Serial.println("SURESTE o 135");  // 135 grados
  direccion= 135;

  if((vel>272) && (vel<301))
  //Serial.println("SUR o 180 GRADOS");  // 180 grados
  direccion= 180;

  if((598<vel) && (vel<661))
  //Serial.println("SUROESTE o 225");  // 225 grados
  direccion= 225;

  if((vel>897) && (vel<991))
  //Serial.println("OESTE o 270 GRADOS");  // 270 grados
  direccion= 270;

  if((842<vel) && (vel<930))
  //Serial.println("NOROESTE o 315");  // 315 grados
  direccion= 315;
  

  // Envio del dato DIRECCION VIENTO a Rx
  Serial.print('I');
  Serial.print(direccion);
  Serial.println('#');
  Led();

  delay(tiempo);


  //================================================
  // PLUVIOMETRO

  // Lectura del sensor de lluvia
if (millis()<tiempohora) {
    tiempohora=0;
  }
  if (millis()-tiempohora > 3600000UL) {
    tiempohora=millis();
    litros=ISRcontador * 0.27945;
    ISRcontador=0;
    contador=0;
  }

    // Envio del dato CANTIDAD DE LLUVIA a Rx
    Serial.print('J');
    Serial.print(litros, 0); // Las unidades son mm/m²
    Serial.println('#');
    Serial.println();
    Led();

    delay(tiempo);

  } 
















