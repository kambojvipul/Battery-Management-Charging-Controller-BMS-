// This task transmits 28 battery/ charge parameters to ESP32c3 using UART, which it further transmits on BLE, 
// It receives the Two Parameters of DC Switch and Inv Switch from ESP32C3.  

#define uart_rx_pin 13
const uint8_t length = 30; // Toatl Variables to be sent 28 and 2 are start & end values (-9999) for tx

String str = "";
String receivedData = ""; // Received data from Mobile App 
int intArray[length];  //Tx array
int invArray[4] = {0,0,0,0}; //int invArray[3] = {0,0,0};  //Rx Array

///////////Set Up Task///////////

void HMI_task_setup()
{
  Serial.println("This is HMI Task Setup.");
  Serial1.begin(115200);//, SERIAL_8N1, -1, -1, false 20000UL, 6);
  //Serial1.setRxBufferSize(6);
  delay(100);
  while (!Serial1); //it is for Esp32 Ble
  
}

////////////////Main Freertos Task Body///////////////////
static void HMI_task(void *pvParameters)
{
  while(1)
  {
  
  read_string(); // Read String Function which includes Receiving and Transmitting Function Inside

  //taskYIELD();
  myDelayMs(2000); // This task will remain suspended for 2 seconds
  }
}


void transmit_data(void) // Transmit data via UART
{
    para_to_intArray(); //convert all parameters to integer array
  str = int_to_String(intArray, length); // convert integer array to string to be transmitted
  Serial.print("Tx: "); //It is for terminal
  Serial.println(str); //It is for terminal
  Serial1.println(str); //It is for ESP32 Ble

}


void stringToIntArray(String str, int intArray[], int size)  //general function to convert String to Integer Array
{
  int index = 0;
  int startIndex = 0;
  int endIndex = 0;

  while (endIndex >= 0 && index < size) 
  {
    endIndex = str.indexOf(',', startIndex);
    if (endIndex == -1) 
    {
      intArray[index] = str.substring(startIndex).toInt();
    } 
    else 
    {
      intArray[index] = str.substring(startIndex, endIndex).toInt();
      startIndex = endIndex + 1;
    }
    index++;
  }
}


void read_string() // read uart data of in_switch and dc12_switch, and transmit too after read
{
  
  while(Serial1.available()>0)
  {
   char inChar = Serial1.read(); //read live incoming characters

   if(inChar == '\n') // when detect last character in incoming stream
   {
     Serial.print("Received: ");
     Serial.println(receivedData);
     
     stringToIntArray(receivedData, invArray, 4); // Convert the string to integer array
     if(invArray[0] == -9999 && invArray[3] == -9999) {inv_switch = invArray[1]; dc12v_switch = invArray[2];} //store in switch variables
     
     receivedData = "";
     transmit_data(); 
   }
   else
   receivedData += inChar; //store incoming store here
  }
}

 String int_to_String(int sendInt[], uint8_t length) // general function to convert integer in string
 {
  String result = "";
  
  for(uint8_t t=0; t< length; t++)
  {
    result += String(sendInt[t]);
    if(t <length-1) result += ",";
  }
  return result;
 }


void para_to_intArray(void) // put all values into int array
{
    intArray[0] = -9999;  //start value
    intArray[1] = 111;//(int) bat_soc;
    intArray[2] = (int) bat_volt;
    intArray[3] = (int) bat_current;
    intArray[4] = (int) bat_dis_cycle;
    intArray[5] = (int) bat_energy;
    intArray[6] = (int) bat_minTemp;
    intArray[7] = (int) bat_maxTemp;
    intArray[8] = (int) bat_connection;
    intArray[9] = (int) NTC1;
    intArray[10] = (int) NTC2;
    intArray[11] = (int) NTC3;
    intArray[12] = (int) NTC4;
    intArray[13] = (int) NTC5;
    intArray[14] = (int) box_temp;

    intArray[15] = (int) obc_com_rxerr;
    intArray[16] = (int) ch_volt_cmd;
    intArray[17] = (int) ch_curr_cmd;
    intArray[18] = (int) ch_volt_fb;  
    intArray[19] = (int) ch_curr_fb;
    intArray[20] = (int) OBC_dto_code;
    intArray[21] = (int) charge_temp;
    intArray[22] = (int) freq_avg;

    intArray[23] = (int) max_charge_curr;
    intArray[24] = (int) duty_avg;//charge_duty_cyc;
    intArray[25] = (int) ch_comb_err;
    intArray[26] = (int) freq_avg;//charge_time_left;

    intArray[27] = (int) inv_switch;
    intArray[28] = (int) charge_start_flag;//dc12v_switch;
    intArray[29] = -9999; // end value
}


