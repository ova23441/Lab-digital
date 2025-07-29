// Universidad del Valle de Guatemala
// BE3029 - Electrónica Digital 2
// Dulce Ovando
// Lab. 2 - Contador con modos (décadas / binario)

// === Librerías ===
#include <Arduino.h>
#include <stdint.h>

// === Definiciones de pines ===
// LEDs 
constexpr uint8_t LED1 = 12;
constexpr uint8_t LED2 = 14;
constexpr uint8_t LED3 = 27;
constexpr uint8_t LED4 = 26;

// Botones
constexpr uint8_t BTN_SUMAR = 5;   // Pulldown 
constexpr uint8_t BTN_RESTAR = 19; // Pullup
constexpr uint8_t BTN_MODO  = 4;   // Pullup

// === Parámetros de antirrebote ===
constexpr unsigned long REBOTE_MS = 50;  // Tiempo para ignorar rebotes (ms)

// === Modos de operación ===
enum ModoContador {
  DECADAS,
  BINARIO
};

// Límites para loS modos
constexpr int LIMITE_DECADAS = 4;
constexpr int LIMITE_BINARIO = 15;

// === Variables de estado ===
ModoContador modo = DECADAS;  // Modo inicial: décadas
int cuenta = 0;               // Valor del contador

// Variables para anti-rebote
unsigned long tSum = 0, tRes = 0, tMod = 0;
bool estSum = LOW,  ultSum = LOW;   // BTN_SUMAR: pulldown (reposo LOW)
bool estRes = HIGH, ultRes = HIGH;  // BTN_RESTAR: pullup (reposo HIGH)
bool estMod = HIGH, ultMod = HIGH;  // BTN_MODO: pullup (reposo HIGH)

// ------------------ Funciones ------------------

bool botonPresionado(uint8_t pin, bool activoBajo,
                     unsigned long &tCambio,
                     bool &estadoEstable,
                     bool &ultimaLectura)
{
  bool lectura = digitalRead(pin);
  unsigned long ahora = millis();
  bool evento = false;

  // Detector de cambio de lectura 
  if (lectura != ultimaLectura) {
    tCambio = ahora;           // tiempo del cambio
    ultimaLectura = lectura;   // última lectura
  }

  // Actualizador de estado estable
  if ((ahora - tCambio) > REBOTE_MS) {
    if (lectura != estadoEstable) {
      estadoEstable = lectura;
      // Comprobacion de lectura de botón 
      bool presionado = activoBajo ? (estadoEstable == LOW) : (estadoEstable == HIGH);
      if (presionado) evento = true; 
    }
  }

  return evento;
}

// Actualizar el estado de los 4 LEDs con valores booleanos
void fijarLeds(bool v1, bool v2, bool v3, bool v4) {
  digitalWrite(LED1, v1);
  digitalWrite(LED2, v2);
  digitalWrite(LED3, v3);
  digitalWrite(LED4, v4);
}

// Muestrar el número en modo décadas
void mostrarDecadas(int n) {
  fijarLeds(n == 1, n == 2, n == 3, n == 4);
}

// Muestrar el número en modo binario
void mostrarBinario(uint8_t n) {
  digitalWrite(LED1, n & 0x01);
  digitalWrite(LED2, (n >> 1) & 0x01);
  digitalWrite(LED3, (n >> 2) & 0x01);
  digitalWrite(LED4, (n >> 3) & 0x01);
}

// Actualiza los LEDs según el modo y el valor de cuenta
void actualizarLeds() {
  if (modo == BINARIO) {
    mostrarBinario((uint8_t)cuenta);
  } else {
    mostrarDecadas(cuenta);
  }
}

// Actualiza el valor de la cuenta con el incremento o decremento
void actualizarCuenta(int delta) {

  cuenta += delta;

  if (modo == DECADAS) {
    if (cuenta > LIMITE_DECADAS) cuenta = 0;
    else if (cuenta < 0) cuenta = LIMITE_DECADAS;
  } else { // BINARIO
    if (cuenta > LIMITE_BINARIO) cuenta = 0;
    else if (cuenta < 0) cuenta = LIMITE_BINARIO;
  }

  actualizarLeds();
}


// ------------------ Configuración inicial ------------------
void setup() {
  // Configurar LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  fijarLeds(LOW, LOW, LOW, LOW);

  //========== Configurar botones =========
  pinMode(BTN_SUMAR, INPUT_PULLDOWN); // Botón sumar (pulldown)
  pinMode(BTN_RESTAR, INPUT_PULLUP);  // Botón restar (pullup)
  pinMode(BTN_MODO, INPUT_PULLUP);    // Botón cambio de modo (pulldown)
}

// ------------------ Bucle principal ------------------
void loop() {
  // Cambiador de modo
  if (botonPresionado(BTN_MODO, true, tMod, estMod, ultMod)) {
    modo = (modo == DECADAS) ? BINARIO : DECADAS;
    cuenta = 0;         // reinicia la cuenta al cambiar modo
    actualizarLeds();
  }

  // Incrementar si se presiona botón sumar
  if (botonPresionado(BTN_SUMAR, false, tSum, estSum, ultSum)) {
    actualizarCuenta(1);
  }

  // Decrementar si se presiona botón restar
  if (botonPresionado(BTN_RESTAR, true, tRes, estRes, ultRes)) {
    actualizarCuenta(-1);
  }
}
