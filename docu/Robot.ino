
/*****************************************************************************/
/***                                                                       ***/
/*** Version: 1.0                                                          ***/
/*** Datum: 15.02.2013                                                     ***/
/***                                                                       ***/
/*** Dies ist der Programmcode für das Starter Kit von Florian H.          ***/ 
/***                                                                       ***/
/***                                                                       ***/
/***                                                                       ***/
/***                                                                       ***/
/*** Weiter-Entwicklung erwünscht! Aber bitte erzählt uns davon... ;-)     ***/
/***                                                                       ***/
/***                                                                       ***/
/*****************************************************************************/

// Nützliche Funktionen für die Servo Ansteuerung kommen von hier:
#include <Servo.h> 

// Welcher Pin hat welche Funktion???
//      Funktion  Pin  Beschreibung
#define IR_SENSOR 2    // Der Sharp IR Sesor ist an Pin 2
#define DIR_B     13   // Richtung B
#define DIR_A     12   // Richtung A
#define PWM_B     11   // Geschwindigkeit B
#define BRAKE_A   9    // Bremse A
#define BRAKE_B   8    // Bremse B
#define PWM_A     3    // Geschwindigkeit A
#define SERVO     5    // Servo wird an Pin 5 angesteuert

// Nützliche Einstellungen:
#define FULL_SPEED        128             // Vollgas ist 255, höher geht nicht. Erstmal nur die halbe Geschindigkeit...
#define TURN_SPEED        255             // schnell Drehen
#define LEFT              LOW             // Links drehen bei LOW
#define RIGHT             HIGH            // ... und Rechts bei HIGH
#define CLOSEST_DISTANCE  350             // Bei diesem Wert am IR Sensor soll der Roboter anhalten --> Ist der Wert KLEINER hält der Roboter FRÜHER an
#define SERVO_RIGHT       55              // Mit welchem Winkel stößt der Sensor auf dem Servo Rechts an?
#define SERVO_LEFT        125             // Das gleiche mit Links

Servo SensorServo;  // Mit diesem Element wird der Servo gesteuert

// An welcher Servo Position ist die Distanz zur Gefahr am geringsten?
int SeekingPositionWithClosestDanger()
{
  // Erst mal anhalten
  digitalWrite(BRAKE_A, HIGH); 
  digitalWrite(BRAKE_B, HIGH);
  int ServoPosition;
  int MinimumDistance = 0;
  int MinimumServoPosition = 0;
 
  // Von Rechts (SERVO_RIGHT = 55) nach Links (SERVO_LEFT=125) alle Servo Positionen anfahren. mit ServoPosition++ wird Wert immer um 1 erhöht
  for(ServoPosition = SERVO_RIGHT;ServoPosition <= SERVO_LEFT; ServoPosition++)
  {
    // Servo Wert einstellen
    SensorServo.write(ServoPosition);
    delay(10);
    // ist der aktuelle Wert näher, als der minimale Wert?
    if(analogRead( IR_SENSOR ) > MinimumDistance )
    {
      // Ja: aktueller Wert ist neues Minimum
      MinimumDistance = analogRead( IR_SENSOR );
      // Außerdem merken wir uns die Servo Position
      MinimumServoPosition = ServoPosition; 
    }   
  }
  // Die gefundene Position wieder einstellen und Wert zurückgeben
  SensorServo.write(MinimumServoPosition);
  return MinimumServoPosition;
}

byte ServoPosition = 90;
boolean TurnServo = RIGHT;
// Vorwärts fahren und dabei den Sensor hin und her schwenken
void DriveForward()
{
  SensorServo.write( ServoPosition );
  //Beide Motoren auf Geradeaus stellen, ...
  digitalWrite( DIR_A, HIGH );
  digitalWrite( DIR_B, HIGH );
  
  // ... Vollgas und ...
  analogWrite( PWM_A, FULL_SPEED );
  analogWrite( PWM_B, FULL_SPEED );
  
  // ..Bremsen lösen!
  digitalWrite( BRAKE_A, LOW );
  digitalWrite( BRAKE_B, LOW );
  
  // Dreht sich der Servo nach Rechts?
  if( TurnServo == LEFT )
    ServoPosition = ServoPosition+1;  // Weiter nach Rechts drehen. d.h. Wert um 1 erhöhen
  if( TurnServo == RIGHT ) 
    ServoPosition = ServoPosition-1;  // Sonst nach Links drehen, also 1 vom Wert abziehen
    
  // Hat der Servo das Linke Ende erreicht?
  if( ServoPosition > SERVO_LEFT )
    TurnServo = RIGHT; // Jetzt nach Rechts drehen
  if( ServoPosition < SERVO_RIGHT )
    TurnServo = LEFT;  // Sonst nach Links drehen...
}

// Drehen! Aber in welche Richtung??? LEFT für Links(gegen den Uhrzeiger) RIGHT für Rechts (im Uhrzeigersinn)
void Turn( boolean Direction )
{
  // Bremsen
  digitalWrite( BRAKE_A, HIGH ); 
  digitalWrite( BRAKE_B, HIGH );
  delay( 500 );

  digitalWrite( DIR_A, Direction );    // Motor A in die "RICHTUNG" drehen
  digitalWrite( DIR_B, !Direction );   // Motor B in entgegen der "RICHTUNG" drehen
   
  // Geschwindigkeit für das Drehen einstellen
  analogWrite( PWM_A, TURN_SPEED );
  analogWrite( PWM_B, TURN_SPEED );
  
  // Bremsen lösen
  digitalWrite( BRAKE_A, LOW );
  digitalWrite( BRAKE_B, LOW );
  
  // Solange drehen, bis der Sensor eine 10% weitere Entfernung misst
  while( ( CLOSEST_DISTANCE * 1 ) < analogRead( IR_SENSOR ) )
  {
    delay( 50 );
  }
  // Halt!
  digitalWrite( BRAKE_A, HIGH ); 
  digitalWrite( BRAKE_B, HIGH );
  delay(1000);
}

// Bevor es los geht... Diese Funktion wird am Anfang genau einmal ausgeführt
void setup( )
{
  //Motor A (rechts) initialisieren
  pinMode( DIR_A, OUTPUT );    // Pin für Richtung Motor A als Ausgang definieren
  pinMode( BRAKE_A, OUTPUT );  // Pin für Bremse Motor A als Ausgang definieren
  //Motor B (links) initialisieren
  pinMode( DIR_B, OUTPUT );    // Pin für Richtung Motor B als Ausgang definieren
  pinMode( BRAKE_B, OUTPUT );  // Pin für Bremse Motor B als Ausgang definieren
  
  // Beide Bremsen anziehen, HIGH = Bremsen!
  digitalWrite( BRAKE_A, HIGH );
  digitalWrite( BRAKE_B, HIGH );

  // Servo initialiseren und auf 90° stellen
  SensorServo.attach( SERVO );
  SensorServo.write( 90 );
  delay( 500 );

  // Warten bis etwas vor dem Sensor ist
  while( CLOSEST_DISTANCE  > analogRead( IR_SENSOR ) )
  {
    // solange warten...
    delay( 100 );
  }
  
  // Los geht's!!!
}

// Der eigentliche Programmablauf. Nachdem setup() fertig ist wird die Funktion loop() endlos nacheinander ausgeführt
void loop( )
{
  int DangerPosition;
  int Distance;
  
  Distance = analogRead( IR_SENSOR );    // was misst der Sensor?
  if(Distance > CLOSEST_DISTANCE )       // nah genug? Ist ein Hindernis nah, kommt ein großer Wert zurück. Sonst ein kleiner.
  {
    DangerPosition = SeekingPositionWithClosestDanger();  // Nochmal alles scannen um zu wissen, wo genau die Gefahr am nächsten ist...
    
    // Gefahr Links?
    if( DangerPosition <= 90 )
    {
      Turn( RIGHT );  // Rechts drehen
    }
    // Oder doch Rechts?
    if( DangerPosition > 90 )
    {
      Turn( LEFT );   // dann Links drehen
    }
  }
  DriveForward();  // Immer gerade aus!!!
  delay( 10 );
}
