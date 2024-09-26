#include <CustomQTRSensors.h>
#include <Servo.h>

#define NUM_SENSORS 4
#define TIMEOUT 2000
#define EMITTER_PIN 2

QTRSensorsRC qtrrc((unsigned char[]) {A0, A1, A2, A3}, NUM_SENSORS, TIMEOUT, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
unsigned int position;

const int motorSolPin1 = 3;
const int motorSolPin2 = 4;
const int motorSagPin1 = 5;
const int motorSagPin2 = 6;

Servo damperServo;
Servo escMotor;
const int servoPini = 9;
const int escPin = 10;

const int trigPini = 11;
const int echoPini = 12;

int motorHiziSol = 100;
int motorHiziSag = 100;

unsigned long gecikmeSuresi = 2000;
bool damperAcik = false;

void setup() {
  Serial.begin(9600);

  pinMode(motorSolPin1, OUTPUT);
  pinMode(motorSolPin2, OUTPUT);
  pinMode(motorSagPin1, OUTPUT);
  pinMode(motorSagPin2, OUTPUT);

  damperServo.attach(servoPini);
  escMotor.attach(escPin);

  escMotor.writeMicroseconds(1000);
  delay(2000);

  pinMode(trigPini, OUTPUT);
  pinMode(echoPini, INPUT);

  damperServo.write(0);
}

void loop() {
  if (copTespit()) {
    cizgiTakip();
  }

  position = qtrrc.readLine(sensorValues);
  for (unsigned char i = 0; i < NUM_SENSORS; i++) {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println(position);
  Serial.println(copTespit() ? "Çöp algılandı" : "Çöp algılanmadı");

  delay(500);
}

bool copTespit() {
  digitalWrite(trigPini, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPini, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPini, LOW);

  long sure = pulseIn(echoPini, HIGH);
  long mesafe = sure * 0.034 / 2;

  Serial.print("Mesafe: ");
  Serial.print(mesafe);
  Serial.println(" cm");

  if (mesafe < 10) {
    return true;
  } else {
    return false;
  }
}

void cizgiTakip() {
  if (copTespit()) {
    motorlariDurdur();
    copBosalt();
    return;
  }

  if (position > 200) {
    motorHiziAyarla(motorHiziSol, motorHiziSag);
  } else if (sensorValues[0] > 200) {
    motorHiziAyarla(motorHiziSol / 2, motorHiziSag);
  } else if (sensorValues[3] > 200) {
    motorHiziAyarla(motorHiziSol, motorHiziSag / 2);
  } else {
    motorHiziAyarla(motorHiziSol, motorHiziSag);
  }
}

void motorHiziAyarla(int solHiz, int sagHiz) {
  analogWrite(motorSolPin1, solHiz);
  analogWrite(motorSolPin2, 0);
  analogWrite(motorSagPin1, sagHiz);
  analogWrite(motorSagPin2, 0);
}

void motorlariDurdur() {
  analogWrite(motorSolPin1, 0);
  analogWrite(motorSagPin1, 0);
}

void copBosalt() {
  Serial.println("Çöp boşaltma işlemi başlatılıyor...");

  damperServo.write(90);
  delay(2000);

  Serial.println("ESC motoru çalıştırılıyor...");
  for (int speed = 1000; speed <= 1050; speed += 10) {
    escMotor.writeMicroseconds(speed);
    delay(200);
  }

  delay(3000);

  Serial.println("ESC motoru durduruluyor...");
  escMotor.writeMicroseconds(1000);
  delay(1000);

  damperServo.write(0);
  delay(2000);

  Serial.println("Çöp boşaltma işlemi tamamlandı.");
}