#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
    
#define AccesFlag_PIN 2 // led rouge , vs pouvez pas passez 
#define Gate_PIN 3 // led vert : vous pouvez passez et seulement avec ouverture de la porte 
#define Max_Acces 3 // nbr d'essais maximal 

byte Count_acces=0; 
byte CodeVerif=0; 
byte Code_Acces[4]={0x20, 0x12, 0x23, 0x2B}; 
char data ; 
//String id ; 


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

// Init array that will store new NUID 
byte nuidPICC[4];

void setup() 
{ 
  // Init RS232
  Serial.begin(9600);

  // Init SPI bus
  SPI.begin(); 

  // Init MFRC522 
  rfid.PCD_Init(); 

  // Init LEDs 
 pinMode(AccesFlag_PIN, OUTPUT);
  pinMode(Gate_PIN, OUTPUT);
  
  digitalWrite(AccesFlag_PIN, LOW);
  digitalWrite(Gate_PIN, LOW);
}
 
void loop() 
{

   //digitalWrite(Gate_PIN, HIGH);
  // Initialisé la boucle si aucun badge n'est présent 
  if ( !rfid.PICC_IsNewCardPresent())
    return;

  // Vérifier la présence d'un nouveau badge 
  if ( !rfid.PICC_ReadCardSerial())
    return;
/*id=PICC_ReadCardSerial;
Serial.println(id);*/

  // Afffichage 
  Serial.println(F("Un badge est détecté"));

  // Enregistrer l’ID du badge (4 octets) 
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  // Vérification du code 
  CodeVerif= GetAccesState(Code_Acces,nuidPICC); 
  if (CodeVerif!=1)
  {  
    
    
    
    Count_acces+=1;
    if(Count_acces==Max_Acces)
    {
     // Dépassement des tentatives (clignotement infinie) 
     while(1)
     { 
      data="s";
      
      digitalWrite(AccesFlag_PIN, HIGH);
      //delay(200); 
      digitalWrite(AccesFlag_PIN, LOW);
      delay(200); 
      // Affichage 
      Serial.println("Alarme!");
     }
    }
    else
    {   
      data="n";
     
      // Affichage 
      Serial.println("Code érroné");
    
      // Un seul clignotement: Code erroné 
      digitalWrite(AccesFlag_PIN, HIGH);
      //delay(1000); 
      digitalWrite(AccesFlag_PIN, LOW);
    }
  }
  else
  {
    // Affichage 
    data = "v";
    Serial.println("Ouverture de la porte");
    
    // Ouverture de la porte & Initialisation 
    digitalWrite(Gate_PIN, HIGH);
    delay(3000); 
    digitalWrite(Gate_PIN, LOW);
    Count_acces=0; 
  }

  // Affichage de l'identifiant 
  Serial.println(" L'UID du tag est:");
  for (byte i = 0; i < 4; i++) 
  {
    Serial.print(nuidPICC[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Re-Init RFID
  rfid.PICC_HaltA(); // Halt PICC
  rfid.PCD_StopCrypto1(); // Stop encryption on PCD
}

byte GetAccesState(byte *CodeAcces,byte *NewCode) 
{
  byte StateAcces=0; 
  if ((CodeAcces[0]==NewCode[0])&&(CodeAcces[1]==NewCode[1])&&
  (CodeAcces[2]==NewCode[2])&& (CodeAcces[3]==NewCode[3]))
    return StateAcces=1; 
  else
    return StateAcces=0; 
}
