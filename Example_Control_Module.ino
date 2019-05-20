 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// Name of Project: Example_Control_Module
/////// Author: Autumn-Storm McFaul
/////// Date Published: 5/20/2019
/////// Description: an example of code for a control module to use with Hydroforya, a Modular Automated Hydroponic 
/////// System. There are descriptions below on the recommended feilds to edit for the custom control element.
/////// Please use the example sensor code for a sensor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  #include <Base64.h>
 
  char buff = ' ';

  ////// ////// vvvvv USER EDIT THESE SECTIONS AS INSTRUCTED BELOW FOR CUSTOMIZATION OF MODULE vvvvv ////// //////
  
  // What would you like to call this Module? place in "" appostrophes 
  String ModuleName = "LED";

  // call this module an number you have not used before in a module between 1-16. place after the equals sign below.
  char Alias;
  
  // is this a controlled element or a sensor? For a senosr, place a 2 after the equals sign. For a contolled element, place a 2.
  char  ModuleType = 2;
  
  // how many values will be controlled/monitored? place after the equals sign below.
  char  NumberofValues = 1;

  //int  AppElements = 1;
  // Will this pair with another module? if so, corrilate numbers so that 1 will pair with 1, 2 will pair with 2, etc. 
  // make sure the same number is in the desired shared module for AssocType as this module
  // place after the equals sign below.
  char  AssocType = 1;
  
  // What is the name of this first value controlled/monitored?
  String  ValueName1 = "Light";
  
  // how big is this value? will it be more than the value of 255? if not, place 1 in the brackets.
  // if more than 255, but under 65,535, place 2 in the brackets
  // if more than 65,535 but under 16,777,215, place 3 in the brackets
  // if more than 16,777,215 but under 4,294,967,296, place 4 in the brackets
  // all else: check if 2^(8*valuesize) = above desired number
  // place after the equals sign below.
  char  ValueSize = 1;
  
  // what is the maximum range for this value? place after the equals sign below.
  int  MaxRange = 99;
  
  // what is the minimum range for this value? place after the equals sign below.
  int  MinRange = 0;
  
  // what will this unit increment by? 
  // (it is recomended that for a range of more than 50, should increment by 10's or more for faster response)
  // example: a range of 100 will take 10 seconds to increment by 10
  // place after the equals sign below.
  char  TolRes = 16;
  
  // What is the starting value for the control element? A resonable setpoint for a sensor?
  // place after the equals sign below.
  int  InitialValue = 11;
  
  // What is the fastest interval the values can be changed in the control element/sensed in the sensor? place a value represented as milliseconds
  // place after the equals sign below.
  int  MinTime = 13;
  
  // What will be the default time interval that values of the module can be requested? place a value represented as milliseconds
  // place after the equals sign below.
  int  RecTime = 15;

  // set a style of element to show in the Andriod application for this module
  // choose 1 for a binary button, 2 for a slider, and 3 for a feild to change
  // this feild is not considered if it is a sensor
  char  AppET = 3;

 // how quickly do you suggest this module change levels? What "step" should the value change?
  int Step = 1;

  // 
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  String endofname = "~";
  char endofpacket = '@';

  // direct is the value being output by the code to the control element to manipulate its levels
  char Direct =  InitialValue;
  int packetnumber;

  // end point
  char EP;
  // conversation ID
  char CID; ////CHAS changed from 2 byte value to 1 byte character

  // input packet for translating
  char ip[20];
  char r;
  
  char inputpack[10];
  
  String outputpack;

  // used to create a pwm wave to the LED grow lamp
  // Direct is manipulated by the command packet from the controller
  float hw = Direct/10;
  float lw = 10-hw;
  float pr;

  char datashow='0';
  char type;
 
// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize pin 11 for led output
  // this is where you may place a pin number that is connected to the sensor/control element
  pinMode(11, OUTPUT);
  
  // for UART communication
  Serial1.begin(9600);

}

 
// the loop function runs over and over again forever
void loop() {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// controller function and command value send-out section

  // manipulate control system value to one the system will understand and execute
   hw = Direct/10;
   lw = 10-hw;

   // delay the time for desired square wave output
   // this is a brute-force PWM, below is an example of a 
  digitalWrite(11, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(hw);              // wait first DU percent high in 10ms fraction
  digitalWrite(11, LOW);    // turn the LED off by making the voltage LOW
  delay(lw);              // wait las DU percent low in 10ms fraction
  
  // may be done via interrupt as well:
  // hwintp = hw/10;
  // bitsdc = hwintp*255;
  // analogWrite(analogpin, bitsdc);
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// read in packet section

   // if there is a message incoming
   if(Serial1.available()>0){
     
     r = 0;
     int i=0;

    // while reading in
     while(Serial1.available()>0){

            // place values in an array. the array will be no more than 20 characters, or will be cutoff
            r = Serial1.read();
            ip[i] = r; 
            i++;
            delay(1);
      }
      
      // decode the message for the microcontroller to use as needed
      // the message will be in base64
      base64_decode(inputpack, ip, i);

//////////////////////////////////////////////////////////////////////////
// interpreting section

     // first incoming packet tells the type of packet to respond to
      type = inputpack[0];

      // end point is the next value in the packet. This is translated into the correct value via char
      EP = (char)inputpack[1];
   }

          else
          {
             // if there is no packet, do nothing and check again whilest sending pwm wave to control the element
            return;
          }
          
    
    if(type==3 || type==5){
      
        CID = inputpack[2]; 
        Alias = inputpack[3];

    }
    
    if(type==5){

      // collect value to control the element from the command packet
      Direct = inputpack[4];
       
    }


//////////////////////////////////////////////////////////////////////////
// packet creation and sendoff section

        // if an identification request packet is recieved
        if(type==1){

            Serial.print("EP char: "); Serial.println((byte)EP);
            Serial.print("EP String: "); Serial.println(String(EP));
          
          outputpack = char(2);
          outputpack += EP;
          outputpack += ModuleType;
          outputpack += NumberofValues;
          String MN = checkcharfn(ModuleName);          
          outputpack += MN;
          outputpack += endofname;
          String VN = checkcharfn( ValueName1);
          outputpack += VN;
          outputpack += endofname;
          //outputpack += Alias;
          outputpack += AssocType;
          outputpack += ValueSize;
          outputpack += char(MinRange);
          outputpack += char(MaxRange);
          outputpack += char(Direct);
          outputpack += char(InitialValue);
          outputpack += char(Step);
          outputpack += char((RecTime & 65280)>>8);
          outputpack += char((RecTime & 255));
          outputpack += char((MinTime & 65280)>>8);
          outputpack += char((MinTime & 255));
          outputpack += TolRes;
          outputpack += AppET;

          int sp = outputpack.length();
          char op[sp];

          for(int i=0;i<sp;i++){
            op[i]=outputpack.charAt(i);
          }
          
          int EL = base64_enc_len(sp);
          char B64P[EL+2]; 
          base64_encode(B64P, op, sp);
          
          B64P[EL] = endofpacket;
          B64P[EL+1] = '\0'; //CHAS changed from '\r' to '\0'

          Serial1.print(B64P);
          Serial.print("Sending to the VDIP1: "); Serial.println(B64P);
          
          type='0';

          return;
        }

        if(type==3){
        
          outputpack = char(4); 
          outputpack += EP;
          outputpack += CID;
          outputpack += Alias;
          outputpack += Direct;
          
          int sp = outputpack.length();
          char op[sp];

          for(int i=0;i<sp;i++){
            op[i]=outputpack.charAt(i);
          }
          
          int EL = base64_enc_len(sp);
          char B64P[EL+2]; 
          base64_encode(B64P, op, sp);
          
          B64P[EL] = endofpacket;
          B64P[EL+1] = '\0'; //CHAS changed from '\r' to '\0'

          Serial1.print(B64P);

          type='0';

          return;
          }

        if(type==5){

          outputpack = char(6);
          outputpack += EP;
          outputpack += CID;
          outputpack += Alias;
          outputpack += char(1);
          
          int sp = outputpack.length();
          char op[sp];

          for(int i=0;i<sp;i++){
            op[i]=outputpack.charAt(i);
          }
          
          int EL = base64_enc_len(sp);
          char B64P[EL+2]; 
          base64_encode(B64P, op, sp);
          
          B64P[EL] = endofpacket;
          B64P[EL+1] = '\0'; //CHAS changed from '\r' to '\0'

          Serial1.print(B64P);
          Serial.print("Sending to the VDIP1: "); Serial.println(B64P);

          type='0';

          return;
          
          }

        if(type!=1 && type!=3 && type!=5 && type!='0'){

          outputpack = byte(6);
          outputpack += EP;
          outputpack += CID;
          outputpack += Alias;
          outputpack += byte(0);

          int sp = outputpack.length();
          char op[sp];

          for(int i=0;i<sp;i++){
            op[i]=outputpack.charAt(i);
          }
          
          int EL = base64_enc_len(sp);
          char B64P[EL+2]; 
          base64_encode(B64P, op, sp);
          
          B64P[EL] = endofpacket;
          B64P[EL+1] = '\r';

          Serial1.print(B64P);
          Serial.print("Sending to the VDIP1: "); Serial.println(B64P);

          type='0';

          return;
          }
}


// failsafe for invalid characters that will harm the code functions
// while also limiting characters so they will fit on the application screen
String checkcharfn(String S){

String ret="";

for(int i=0;i<(15<S.length()?15:S.length());i++){
  if(S[i]=='~'){
    ret+='-';
  }else if(S[i]=='@'){
    ret+='a';
  }else {
    ret+=S[i];
  }
}

return ret;

}
