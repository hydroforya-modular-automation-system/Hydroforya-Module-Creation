  #include <Base64.h>
 
  char buff = ' ';

  ////// ////// vvvvv USER EDIT THESE SECTIONS AS INSTRUCTED BELOW FOR CUSTOMIZATION OF MODULE vvvvv ////// //////
  
  // What would you like to call this Module? place in "" appostrophes 
  String ModuleName = "Photo-sensor";

  // call this module an number you have not used before in a module between 1-16. place after the equals sign below.
  char Alias;// = 1;
  
  // is this a controlled element or a sensor? For a senosr, place a 2 after the equals sign. For a contolled element, place a 2.
  char  ModuleType = 1;
  
  // how many values will be controlled/monitored? place after the equals sign below.
  char  NumberofValues = 1;

  // Will this pair with another module? if so, corrilate numbers so that 1 will pair with 1, 2 will pair with 2, etc. 
  // make sure the same number is in the desired shared module for AssocType as this module
  // place after the equals sign below.
  char  AssocType = 1;
  
  // What is the name of this first value controlled/monitored?
  String  ValueName1 = "Resistance";
  
  // how big is this value? will it be more than the value of 255? if not, place 1 in the brackets.
  // if more than 255, but under 65,535, place 2 in the brackets
  // if more than 65,535 but under 16,777,215, place 3 in the brackets
  // if more than 16,777,215 but under 4,294,967,296, place 4 in the brackets
  // all else: check if 2^(8*valuesize) = above desired number
  // place after the equals sign below.
  char  ValueSize = 2;
  
  // what is the maximum range for this value? place after the equals sign below.
  int  MaxRange = 10000;
  
  // what is the minimum range for this value? place after the equals sign below.
  int  MinRange = 0;

  // what will this unit increment by? 
  // (it is recomended that for a range of more than 50, should increment by 10's or more for faster response)
  // example: a range of 100 will take 10 seconds to increment by 10
  // place after the equals sign below.
  char  TolRes = 10;
  
  // What is the starting value for the control element? A resonable setpoint for a sensor?
  // place after the equals sign below.
  int  InitialValue = 5000;

  int DesiredValue = 5000; //CHAS ADDED THIS
  
  // What is the fastest interval the values can be changed in the control element/sensed in the sensor? place a value represented as milliseconds
  // place after the equals sign below.
  int  MinTime = 9;
  
  // What will be the default time interval that values of the module can be requested? place a value represented as milliseconds
  // place after the equals sign below.
  int  RecTime = 20;

  // set a style of element to show in the Andriod application for this module
  // choose 1 for a binary button, 2 for a slider, and 3 for a feild to change
  // this feild is not considered if it is a sensor
  char  AppET = 2;

  // how quickly do you suggest this module change levels? What "step" should the value change?
  char Step = 100;

  // 
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // for packet use
  String endofname = "~";
  char endofpacket = '@';
  
  // data is the value being sent as a sensor reading
  int Data =  InitialValue;
  int packetnumber;

  // end point
  char EP;
  // conversation ID
  char CID;

  // input packet, for translating
  char ip[20];
  char r;

  char inputpack[10];
  
  String outputpack;

  // used to translate and read in sensor values
  float pr;
  int Resistance;

  char datashow='0';
  char type;
 
// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize pin A1 for potentiometer input
  // this is where you may place a pin number that is connected to the sensor/control element
  pinMode(A0, INPUT);

  // for UART communication
  Serial1.begin(9600);

}

 
// the loop function runs over and over again forever
void loop() {

//////////////////////////////////////////////////////////////////////////////
// sensor read-in section

    // read in sensor value
    // study your sensor, and learn to read in and translate the levels 
    // for the control system, then place the code to do so here
     pr = analogRead(A0);
     Data = abs((9.817*pr) - 95);

///////////////////////////////////////////////////////////////////////////////     
// read in packet section

// if there is an incoming message
   if(Serial1.available()>0){
     
     r = 0;
     int i=0;

   // while reading in
     while(Serial1.available()>0){

            // place values in an array. the array will be no more than 20 characters, or will be cutoff
            r = Serial1.read();
            Serial.println("recieved");
            ip[i] = r; 
            i++;
            // wait for next character
            delay(3);
            // loop until done
      }

      // decode the message for the microcontroller to use as needed
      // the message will be in base64
      base64_decode(inputpack, ip, i);

          else
          {
            // if there is no packet, do nothing and check again whilest reading the sensor value
            return;
          }
          
//////////////////////////////////////////////////////////////////////////
// interpreting section

      // first incoming packet tells the type of packet to respond to
      type = inputpack[0];

      // end point is the next value in the packet. This is translated into the correct value via char
      EP = (char)inputpack[1];

      // if the packet type is declared a command packet, the conversation id and alias assignment must be given back to the 
      // USB interface, and thus stored here
      if(type==3){
        CID = inputpack[2];
        Alias = inputpack[3];
      }
    


//////////////////////////////////////////////////////////////////////////
// packet creation and sendoff section

        // if an identification request packet is recieved
        if(type==1){

          // then send this information below
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
          outputpack += char((MinRange & 65280)>>8);
          outputpack += char((MinRange & 255));
          outputpack += char((MaxRange & 65280)>>8);
          outputpack += char((MaxRange & 255));
          outputpack += char((Data & 65280)>>8);
          outputpack += char((Data & 255));
          outputpack += char((InitialValue & 65280)>>8);
          outputpack += char((InitialValue & 255));
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
          B64P[EL+1] = '\0';

          //Serial1.print(outputpack);
          Serial1.print(B64P);
          
          type='0';

          return;
        }

        if(type==3){
          
  
          outputpack = char(4); 
          outputpack += EP;
          outputpack += CID;
          outputpack += Alias;
          outputpack += char((Resistance & 65280)>>8);
          outputpack += char((Resistance & 255));
           
          int sp = outputpack.length();
          char op[sp];

          for(int i=0;i<sp;i++){
            op[i]=outputpack.charAt(i);
          }
          
          int EL = base64_enc_len(sp);
          char B64P[EL+2]; 
          base64_encode(B64P, op, sp);
          
          B64P[EL] = endofpacket;
          B64P[EL+1] = '\0';

          //Serial1.print(outputpack);
          Serial1.print(B64P);

          type='0';

          return;
          }


        if(type!=1 && type!=3 && type!='0'){

          outputpack = char(6);
          outputpack += EP;
          outputpack += CID;
          outputpack += Alias;
          outputpack += 0;

          int sp = outputpack.length();
          char op[sp];

          for(int i=0;i<sp;i++){
            op[i]=outputpack.charAt(i);
          }
          
          int EL = base64_enc_len(sp);
          char B64P[EL+2]; 
          base64_encode(B64P, op, sp);
          
          B64P[EL] = endofpacket;
          B64P[EL+1] = '\0';

          //Serial1.print(outputpack);
          Serial1.print(B64P);

          type='0';

          return;
          }
}



// failsafe for invalid characters that will harm the code functions
// while also limiting characters so they will fit on the application screen
// if this is altered, the sensor/control element may not function properly
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
