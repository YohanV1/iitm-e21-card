#include "SPI.h"                    //Serial Peripheral Interface. Used to send data between the Arduino and Proximity Reader
#include "MFRC522.h"                //To initialized a variable key, belonging to the scope named MFRC522::MIFARE_Key

int SS_PIN = 10;                    //Initializing variables to indicate which pins the Proximity Reader is connected to from the Arduino
int RST_PIN = 9;
int SP_PIN = 8;

MFRC522 rfid(SS_PIN, RST_PIN);      //Giving the parameters to the Promixity Reader's object of its class , "rfid"
MFRC522::MIFARE_Key key;            //Initializing a variable key belonging to the scope MFRC522::MIFARE_Key

int i;                               //Initializing integer variables. 'i' is used to uniquely distinguish between different UID profiles in an array
int fare;                            //'fare' is used to calculate the cost of going from current to final stop
int d, d1;                           //'d' is the current stop number, 'd1' is the final stop number
int sz, sz1;                         //To obtain the size of the arrays 'dn' and 'ID', respectively

boolean warn = false ;               //a boolean to check when balance is low

String ID[] = {"D9:D4:82:B9" , "E4:E8:12:E2", "89:35:D9:A3", "F4:D2:90:14", "76:53:E2:R2"};   //To store a list of UIDs

String Name[] = {"Yohan Vinu" , "Sashank Krishna", "M.S Siddharth",                           //to store the profile name of the corresponding index's UID in array 'ID'
                 "Mahima Sakthi", "Anuj Devendra"
                };

String dn[] = { "Luz Corner", "Mylapore","Mandaveli","Sathya Studios","Adyar Signal", "Adyar Depot","Thiruvanmiyur","S.R.P Tools", "Tharamani","Barathi Nagar" "Velachery", 
                    //to store the name of a few bus stops/stands in words
               
              };

int bal[] = {500 , 500, 500, 500, 500};                                                       //to store the balances of profiles corresponding to the index's of the profile name and UID

void setup()                                                                                  //method from which the program starts
{
  Serial.begin(9600);                                                                         //opens serial port, sets data to 9600 bits per second
  SPI.begin();                                                                                //Initializes the Serial Peripheral Interface and gets it ready to give outputs
  rfid.PCD_Init();                                                                            //Initializes the MFRC522 chip(Proximity Reader)
  Serial.println("Approximate your card to the reader...");                                   //Output
}

void loop()                                                                                   //method which forms the main body of code, and this is in a continuous loop
{
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())                           //if no card is present or no card can be read, the program goes back to loop()
    return;                                                                                   //to send program back to loop
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);                              //Making a variable to get the type of Proximity Integrated Circuit Card(PICC)

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&                                           //If the card is not one of these types, the program goes back to loop()
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    return;
  }
  String strID = "";                                                                          //a string to hold the UID
  for (byte i = 0; i < 4; i++) {                                                              //iterate over the four bytes of the UID
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +                                               //adds a colon after every pair of hexaxdecimal digits in the form nn:nn:nn:nn
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");
  }
  strID.toUpperCase();                                                                        //converting entire UID to upper case
  rfid.PICC_HaltA();                                                                          //Once scanned, these two lines prevent the same card from being continuously scanned again
  rfid.PCD_StopCrypto1();
  sz = sizeof(dn) / sizeof(String);                                                           //getting the number of indices in arrays 'dn' and 'ID'
  sz1 = sizeof(ID) / sizeof(String);
  for (i = 0; i < sz1 ; i++)                                                                  //a loop to check if the scanned UID is already present
  {
    if (strID.equals(ID[i]))                                                                  //checks with each UID in the system
    {
      for (int z = 0; z < 25; z++)
      {
        Serial.println();
      }
      Serial.println("Current Stop: ");
      while (Serial.available() == 0) { }                                                     //to make the program wait till the user inputs something
      d = Serial.parseInt();                                                                  //to get user input

      if (d > sz || d < 1)                                                                    //to check if the user has inputted wrongly
      {

        for (int z = 0; z < 25; z++)
        {
          Serial.println();
        }
        Serial.println("Invalid Stop Number");
        Serial.println("Please Rescan");
        Serial.println("Approximate your card to the reader...");
        return;
      }

      Serial.println("Final Stop: ");
      while (Serial.available() == 0) { }
      d1 = Serial.parseInt();
      Serial.println();

      if (d1 < 1 || d1 > sz || d1 == d )                                                     //to check if the user has inputted wrongly
      {

        for (int z = 0; z < 25; z++)
        {
          Serial.println();
        }

        Serial.println("Invalid Stop Number");
        Serial.println("Please Rescan");
        Serial.println("Approximate your card to the reader...");

        return;
      }

      fare = abs((d1 - d) * 5);                                                              //to calculate the cost of the trip

      if (fare > bal[i])                                                                     //to check if balance is low
      {
        warn = true;
      }
      else
      {
        bal[i] = bal[i] - fare;                                                              //if balance is not low, this deducts fare from balance
      }
      for (int z = 0; z < 25; z++)
      {
        Serial.println();
      }
      Serial.println("------------------------------------------------------------------------------------");          //to print the user's bill

      Serial.println("                             MTC Smart Card Services                                ");

      Serial.println("Name: " + Name[i]);

      Serial.println("From: " + dn[d - 1]);

      Serial.println("To: " + dn[d1 - 1] );

      Serial.print("Cost: Rs.");
      Serial.println(fare);

      if (warn)                                                                                                       //this runs if balance is less than fair
      {
        Serial.print("Balance: Rs.");
        Serial.println(bal[i]);

        Serial.println("Insufficient balance, please pay in cash.");

        Serial.println("------------------------------------------------------------------------------------");
        delay(10000);                                                                                                 //delay to switch from previous user's bill to rerun the program
        for (int z = 0; z < 25; z++)
        {
          Serial.println();
        }
        Serial.println("Approximate your card to the reader...");
        return;
      }
      else                                                                                                            //this runs if balance is more than fair                                                         
      {
        Serial.print("Balance: Rs.");
        Serial.println(bal[i]);

        Serial.println("Money Deducted");

        Serial.println("------------------------------------------------------------------------------------");
        delay(10000);                                                                                                //delay to switch from previous user's bill to rerun the program
        for (int z = 0; z < 25; z++)
        {
          Serial.println();
        }
        Serial.println("Approximate your card to the reader...");

        return;
      }
      break;                                                                                                         //to break out of the beginning for loop
    }
  }
  Serial.println("Unrecognized card. Please rescan.");                                                               //in case the card is not stored in the system, it loops back
}
