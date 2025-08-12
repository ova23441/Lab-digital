#include <Arduino.h>       
#include <stdint.h>       
#include <ESP32Servo.h>    // Librería especial para controlar servos en ESP32

// ==== Pines LEDs (no tengo RGB, son LEDs separados) ====
constexpr uint8_t LED_ROJO  = 12; // LED color rojo
constexpr uint8_t LED_VERDE = 14; // LED color verde
constexpr uint8_t LED_AZUL  = 27; // LED color azul

// ==== Pines Botones ====
constexpr uint8_t BTN_INTENSIDAD = 5;   // Botón para cambiar la intensidad del LED (B4)
constexpr uint8_t BTN_MODO       = 19;  // Botón para cambiar el modo de funcionamiento (B3)
constexpr uint8_t BTN_DERECHA    = 32;  // Botón para mover servo hacia la derecha (B1)
constexpr uint8_t BTN_IZQUIERDA  = 33;  // Botón para mover servo hacia la izquierda (B2)

// ==== Variables LEDs ====
uint8_t modo_actual = 0; // 0=Rojo, 1=Verde, 2=Azul, 3=Sincronizado con el servo
uint8_t nivel_brillo[3] = {0, 0, 0}; // Índice de nivel de brillo para cada color
const uint8_t niveles[] = {0, 64, 128, 192, 255}; // Valores posibles de brillo (PWM)
const uint8_t num_niveles = sizeof(niveles) / sizeof(niveles[0]); // Cantidad de niveles

bool btn_modo_presionado = false;       // Para evitar lectura repetida del botón MODO
bool btn_intensidad_presionado = false; // Para evitar lectura repetida del botón INTENSIDAD

// ==== Servo ====
Servo miServo;                  // Objeto servo
constexpr uint8_t PIN_SERVO = 25; // Pin donde está conectado el servo
const int posiciones[] = {0, 45, 90, 135, 180}; // Posiciones predefinidas del servo
const uint8_t num_posiciones_servo = sizeof(posiciones) / sizeof(posiciones[0]); // Número de posiciones
uint8_t indice_posicion = 0; // Posición inicial del servo (0°)

bool btn_der_presionado = false; // Estado del botón DERECHA
bool btn_izq_presionado = false; // Estado del botón IZQUIERDA

// ==== Función para apagar todos los LEDs ====
void apagarLEDs() {
  analogWrite(LED_ROJO, 0);
  analogWrite(LED_VERDE, 0);
  analogWrite(LED_AZUL, 0);
}

void setup() {
  // Configuración pines LEDs
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);

  // Configuración botones
  pinMode(BTN_MODO, INPUT_PULLUP);
  pinMode(BTN_INTENSIDAD, INPUT_PULLUP);

  apagarLEDs(); // Apaga los LEDs al iniciar

  // Configuración servo
  miServo.attach(PIN_SERVO, 500, 2500); // Conecta el servo al pin con ancho de pulso de 500-2500 μs
  miServo.write(posiciones[indice_posicion]); // Lleva el servo a la posición inicial
  pinMode(BTN_DERECHA, INPUT_PULLUP);
  pinMode(BTN_IZQUIERDA, INPUT_PULLUP);
}

void loop() {
  // ===== BOTÓN MODO =====
  if (digitalRead(BTN_MODO) == LOW && !btn_modo_presionado) {
    btn_modo_presionado = true;
    modo_actual = (modo_actual + 1) % 4; // Avanza al siguiente modo (0-3)

    // Si entramos al modo sincronizado (modo 3), ajustar LEDs según posición del servo
    if (modo_actual == 3) {
      if (posiciones[indice_posicion] == 0 || posiciones[indice_posicion] == 180) {
        apagarLEDs();
      } else if (posiciones[indice_posicion] == 45) {
        analogWrite(LED_ROJO, 255);
        analogWrite(LED_VERDE, 0);
        analogWrite(LED_AZUL, 0);
      } else if (posiciones[indice_posicion] == 90) {
        analogWrite(LED_ROJO, 0);
        analogWrite(LED_VERDE, 255);
        analogWrite(LED_AZUL, 0);
      } else if (posiciones[indice_posicion] == 135) {
        analogWrite(LED_ROJO, 0);
        analogWrite(LED_VERDE, 0);
        analogWrite(LED_AZUL, 255);
      }
    }
    delay(200); // Anti-rebote
  } else if (digitalRead(BTN_MODO) == HIGH) {
    btn_modo_presionado = false;
  }

  // ===== CONTROL DE BRILLO (SOLO MODOS 0,1,2) =====
  if (modo_actual < 3) {
    if (digitalRead(BTN_INTENSIDAD) == LOW && !btn_intensidad_presionado) {
      btn_intensidad_presionado = true;
      uint8_t &nivel = nivel_brillo[modo_actual]; // Referencia al nivel del LED actual
      nivel = (nivel + 1) % num_niveles; // Cambia al siguiente nivel

      // Aplica el brillo al LED correspondiente
      switch (modo_actual) {
        case 0: analogWrite(LED_ROJO, niveles[nivel]); break;
        case 1: analogWrite(LED_VERDE, niveles[nivel]); break;
        case 2: analogWrite(LED_AZUL, niveles[nivel]); break;
      }
      delay(200); // Anti-rebote
    } else if (digitalRead(BTN_INTENSIDAD) == HIGH) {
      btn_intensidad_presionado = false;
    }
  }

  // ===== CONTROL DEL SERVO =====
  // Movimiento a la derecha
  if (digitalRead(BTN_DERECHA) == LOW && !btn_der_presionado) {
    btn_der_presionado = true;
    if (indice_posicion < num_posiciones_servo - 1) {
      indice_posicion++; // Avanza una posición
      miServo.write(posiciones[indice_posicion]); // Mueve el servo
      delay(300); // Espera movimiento

      // Si estamos en modo sincronizado, cambia LEDs según posición
      if (modo_actual == 3) {
        if (posiciones[indice_posicion] == 0 || posiciones[indice_posicion] == 180) {
          apagarLEDs();
        } else if (posiciones[indice_posicion] == 45) {
          analogWrite(LED_ROJO, 255);
          analogWrite(LED_VERDE, 0);
          analogWrite(LED_AZUL, 0);
        } else if (posiciones[indice_posicion] == 90) {
          analogWrite(LED_ROJO, 0);
          analogWrite(LED_VERDE, 255);
          analogWrite(LED_AZUL, 0);
        } else if (posiciones[indice_posicion] == 135) {
          analogWrite(LED_ROJO, 0);
          analogWrite(LED_VERDE, 0);
          analogWrite(LED_AZUL, 255);
        }
      }
    }
  } else if (digitalRead(BTN_DERECHA) == HIGH) {
    btn_der_presionado = false;
  }

  // Movimiento a la izquierda
  if (digitalRead(BTN_IZQUIERDA) == LOW && !btn_izq_presionado) {
    btn_izq_presionado = true;
    if (indice_posicion > 0) {
      indice_posicion--; // Retrocede una posición
      miServo.write(posiciones[indice_posicion]); // Mueve el servo
      delay(300);

      // Si estamos en modo sincronizado, cambia LEDs según posición
      if (modo_actual == 3) {
        if (posiciones[indice_posicion] == 0 || posiciones[indice_posicion] == 180) {
          apagarLEDs();
        } else if (posiciones[indice_posicion] == 45) {
          analogWrite(LED_ROJO, 255);
          analogWrite(LED_VERDE, 0);
          analogWrite(LED_AZUL, 0);
        } else if (posiciones[indice_posicion] == 90) {
          analogWrite(LED_ROJO, 0);
          analogWrite(LED_VERDE, 255);
          analogWrite(LED_AZUL, 0);
        } else if (posiciones[indice_posicion] == 135) {
          analogWrite(LED_ROJO, 0);
          analogWrite(LED_VERDE, 0);
          analogWrite(LED_AZUL, 255);
        }
      }
    }
  } else if (digitalRead(BTN_IZQUIERDA) == HIGH) {
    btn_izq_presionado = false;
  }
}
