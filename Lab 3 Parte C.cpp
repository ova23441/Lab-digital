#include <Arduino.h>
#include <stdint.h>

// ==== Pines LEDs Contador Manual ====
constexpr uint8_t LED1 = 12;  
constexpr uint8_t LED2 = 14;  
constexpr uint8_t LED3 = 27;  
constexpr uint8_t LED4 = 26;  

// ==== Pines LEDs Contador Automático  ====
constexpr uint8_t LEDA = 25;  
constexpr uint8_t LEDB = 33;  
constexpr uint8_t LEDC = 32;  
constexpr uint8_t LEDD = 4;  

// ==== Pines Botones ====
constexpr uint8_t BTN_SUMAR  = 5;   
constexpr uint8_t BTN_RESTAR = 19;  

// ==== LED de Alarma ====
constexpr uint8_t LED_ALARMA = 21;

// ==== Variables globales ====
volatile bool irqSumar = false;        // Interrupción para sumar
volatile bool irqRestar = false;       // Interrupción para restar
volatile bool tick250ms = false;       // Marca de tiempo de 250 ms

uint8_t cuentaManual = 0;              // Contador de botones
uint8_t cuentaTimer  = 0;              // Contador automático
bool estadoAlarma = false;             // Estado del LED de alarma
bool coincidenciaAnterior = false;     // Para evitar rebotes

// ==== Timer de hardware ====
hw_timer_t* timer = nullptr;

// ==== Funciones de interrupción (ISR) ====
void IRAM_ATTR onSumar()  { irqSumar  = true; }
void IRAM_ATTR onRestar() { irqRestar = true; }
void IRAM_ATTR onTimer()  { tick250ms = true; }

// ==== Mostrar valor binario del contador manual ====
void mostrarBinarioManual(uint8_t n) {
  digitalWrite(LED1, n & 0x01);
  digitalWrite(LED2, (n >> 1) & 0x01);
  digitalWrite(LED3, (n >> 2) & 0x01);
  digitalWrite(LED4, (n >> 3) & 0x01);
}

// ==== Mostrar valor binario del contador automático ====
void mostrarBinarioTimer(uint8_t n) {
  digitalWrite(LEDA, n & 0x01);
  digitalWrite(LEDB, (n >> 1) & 0x01);
  digitalWrite(LEDC, (n >> 2) & 0x01);
  digitalWrite(LEDD, (n >> 3) & 0x01);
}

void setup() {
  // ==== Configuración de pines de salida ====
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LEDA, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDC, OUTPUT);
  pinMode(LEDD, OUTPUT);
  pinMode(LED_ALARMA, OUTPUT);
  digitalWrite(LED_ALARMA, LOW);

  // ==== Configuración de botones e interrupciones ====
  pinMode(BTN_SUMAR, INPUT_PULLDOWN);
  pinMode(BTN_RESTAR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_SUMAR), onSumar, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_RESTAR), onRestar, FALLING);

  // ==== Configuración del timer ====
  timer = timerBegin(0, 80, true); // Prescaler: 80 (1 µs por tick)
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 250000, true); // Cada 250,000 µs = 250 ms
  timerAlarmEnable(timer);

  // ==== Mostrar valores iniciales ====
  mostrarBinarioManual(cuentaManual);
  mostrarBinarioTimer(cuentaTimer);
}

void loop() {
  static unsigned long tLastSum = 0;
  static unsigned long tLastRes = 0;
  unsigned long ahora = millis();

  // ==== Manejador del botón de Sumar ====
  if (irqSumar) {
    irqSumar = false;
    if (ahora - tLastSum >= 50) { // Antirrebote
      cuentaManual = (cuentaManual + 1) & 0x0F;
      mostrarBinarioManual(cuentaManual);
      tLastSum = ahora;
    }
  }

  // ==== Manejador del botón de Restar ====
  if (irqRestar) {
    irqRestar = false;
    if (ahora - tLastRes >= 50) { // Antirrebote 50 ms
      cuentaManual = (cuentaManual - 1) & 0x0F; 
      mostrarBinarioManual(cuentaManual);
      tLastRes = ahora;
    }
  }

  // ==== Incremento automático cada 250 ms ====
  if (tick250ms) {
    tick250ms = false;
    cuentaTimer = (cuentaTimer + 1) & 0x0F; 
    mostrarBinarioTimer(cuentaTimer);
  }

  // ==== Comparación entre contadores y activación de alarma ====
  if (cuentaManual == cuentaTimer) {
    if (!coincidenciaAnterior) {
      estadoAlarma = !estadoAlarma; // Cambiar estado del LED
      digitalWrite(LED_ALARMA, estadoAlarma);

      cuentaTimer = 0; // Reiniciar contador automático
      mostrarBinarioTimer(cuentaTimer);

      coincidenciaAnterior = true; // Evitar repetición
    }
  } else {
    coincidenciaAnterior = false;
  }
}
