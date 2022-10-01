    // CONFIG SERVO
#include<Servo.h>
Servo mekanik;

    // CONFIG KELEMBAPAN TANAH
int sensorPin = A0;   // pin sensor
int powerPin  = 6;    // untuk pengganti VCC
 
    // CONFIG LCD I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

    // CONFIG RTC
#include "RTClib.h"
RTC_DS3231 rtc;
float suhu;
String hari;
int tanggal, bulan, tahun, jam, menit, detik;
char dataHari[7][12] = {"Ahad", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

    // CONFIG TUBIDITY METER AND RELAY
const int relay = 9;
int sensorPinTurbidity = A0;
    
    // MODEL DERAJAT
byte derajat[8] = {
  B00100,
  B01010,
  B01010,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000
};

void kasihPakan(int jumlah) {
    // CONFIG EATING TIMES
  for (int i = 1; i <= jumlah; i++) {
    mekanik.write(150);
    delay(100);
    mekanik.write(0);
    delay(100);
    lcd.init();
  }
}

int bacaSensor() {
    // CONFIG READ SENSOR
  digitalWrite(powerPin, HIGH);             // hidupkan power
  delay(500);
  int nilaiSensor = analogRead(sensorPin);  // baca nilai analog dari sensor
  digitalWrite(powerPin, LOW);
  return 1023 - nilaiSensor;                // makin lembab maka makin tinggi nilai outputnya
}

void setup () {
    // DISPLAY SERIAL MONITOR
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("RTC Tidak Ditemukan");
    Serial.flush();
    abort();
  }

    // SETTING TIME
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    // SETUP LCD
  lcd.init();                       // Initialize the lcd
  lcd.init();                       // Print a message to the LCD.
  lcd.createChar(0, derajat); 

    // SETUP MOTOR SERVO
  mekanik.attach(7);
  mekanik.write(0);

    // SETUP SENSOR KELEMBAPAN TANAH
  pinMode(powerPin, OUTPUT);        // jadikan pin power sebagai output
  digitalWrite(powerPin, LOW);      // default bernilai LOW

    // SETUP SENSOR TURBIDITY
  pinMode(relay, OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
    
}

void loop () {
    // CONFIGURE TIME RTC
  DateTime now = rtc.now();
  hari    = dataHari[now.dayOfTheWeek()];
  tanggal = now.day(), DEC;
  bulan   = now.month(), DEC;
  tahun   = now.year(), DEC;
  jam     = now.hour(), DEC;
  menit   = now.minute(), DEC;
  detik   = now.second(), DEC;
  suhu    = rtc.getTemperature();

    // SHOW TIME AND SUHU IN SERIAL MONITOR
  Serial.println(String() + hari + ", " + tanggal + "-" + bulan + "-" + tahun);
  Serial.println(String() + jam + ":" + menit + ":" + detik);
  Serial.println(String() + "Suhu: " + suhu + " C");
  Serial.println();

    // SHOW TIME AND SUHU IN LCD I2C
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(String() + hari + ", " + tanggal + "-" + bulan + "-" + tahun + " ");
  lcd.setCursor(0, 1);
  lcd.print(String() + jam + ":" + menit + ":" + detik);
  lcd.print(" ");
  lcd.print(suhu);
  lcd.write(byte(0));
  lcd.print("C ");

    // SHOW KELEMBAPAN TANAH IN SERIAL MONITOR
  Serial.print("Nilai kelembaban: ");
  Serial.println(bacaSensor());

    // SHOW KELEMBAPAN TANAH IN LCD I2C
  lcd.setCursor(0,0);
  lcd.print("Nilai = ");
  lcd.print(bacaSensor());
  lcd.print(" RH                ");

    // CONDITION FISH EAT
  if(jam == 12 & menit == 37 & detik == 30){
    kasihPakan(3);
  }

    // PROSES SENSOR TURBIDITY
  int sensorValue = analogRead(sensorPinTurbidity);
  int turbidity = map(sensorValue, 0,640, 100, 0);
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("turbidity:");
  lcd.print("   ");
  lcd.setCursor(10, 0);
  lcd.print(turbidity);
  delay(100);
  if (turbidity < 20) {
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    lcd.setCursor(0, 1);
    lcd.print(" its CLEAR ");
    digitalWrite(relay, LOW);
  }
  if ((turbidity > 10) && (turbidity < 50)) {
    digitalWrite(7, LOW);
    digitalWrite(8, HIGH);
    digitalWrite(9, LOW);
    lcd.setCursor(0, 1);
    lcd.print(" its CLOUDY ");
  }
  if (turbidity > 50) {
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);
    lcd.setCursor(0, 1);
    lcd.print(" its DIRTY ");

    digitalWrite(relay, HIGH);
  }
  
}
