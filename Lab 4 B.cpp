#include <Arduino.h>        
#include <ESP32Servo.h>     // Librería para controlar servos en ESP32

// ==== Pines Servo y Botones ====
constexpr uint8_t PIN_SERVO = 25;       // Pin donde está conectado el servo (señal PWM)
constexpr uint8_t BTN_DERECHA = 32;     // Botón B1 para mover el servo hacia la derecha
constexpr uint8_t BTN_IZQUIERDA = 33;   // Botón B2 para mover el servo hacia la izquierda

Servo miServo; // Objeto para controlar el servo

// ==== Posiciones predefinidas del servo ====
const int posiciones[] = {0, 45, 90, 135, 180}; // Ángulos del servo

const uint8_t num_posiciones = sizeof(posiciones) / sizeof(posiciones[0]); 
// Calcula cuántas posiciones tiene el arreglo anterior

uint8_t indice_posicion = 0; // Posición inicial del servo (índice 0 = 0°)

// Anti-rebote
bool btn_der_presionado = false;
bool btn_izq_presionado = false;

void setup() {
  // Conecta el servo al pin con un rango de pulsos de 500 a 2500 microsegundos
  miServo.attach(PIN_SERVO, 500, 2500);

  // Coloca el servo en la posición inicial (0°)
  miServo.write(posiciones[indice_posicion]);

  // Configuración de los botones
  pinMode(BTN_DERECHA, INPUT_PULLUP);
  pinMode(BTN_IZQUIERDA, INPUT_PULLUP);
}

void loop() {
  // ===== Movimiento hacia la derecha =====
  // Si el botón DERECHA está presionado y no se ha registrado antes
  if (digitalRead(BTN_DERECHA) == LOW && !btn_der_presionado) {
    btn_der_presionado = true; // Marca que está presionado
    if (indice_posicion < num_posiciones - 1) { // Si no está en la última posición
      indice_posicion++; // Avanza una posición
      miServo.write(posiciones[indice_posicion]); // Mueve el servo
      delay(300); // Espera a que se complete el movimiento
    }
  } 
  // Si el botón DERECHA se suelta, reinicia la bandera
  else if (digitalRead(BTN_DERECHA) == HIGH) {
    btn_der_presionado = false;
  }

  // ===== Movimiento hacia la izquierda =====
  if (digitalRead(BTN_IZQUIERDA) == LOW && !btn_izq_presionado) {
    btn_izq_presionado = true;
    if (indice_posicion > 0) { // Si no está en la primera posición
      indice_posicion--; // Retrocede una posición
      miServo.write(posiciones[indice_posicion]); // Mueve el servo
      delay(300);
    }
  } 
  // Si el botón IZQUIERDA se suelta, reinicia la bandera
  else if (digitalRead(BTN_IZQUIERDA) == HIGH) {
    btn_izq_presionado = false;
  }
}
