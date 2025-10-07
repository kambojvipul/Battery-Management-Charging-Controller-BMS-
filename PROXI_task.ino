// This task detect the Plug using Proximity Value and enable/disable CP & OBC Task when plug connected/ disconnected.

#define AVG_PCNT 10
int proxi_volt_pin = A1;    // Proxi Pin (Analog) of Arduino Board
unsigned int proxi_volt[AVG_PCNT] = {0,0,0,0,0,0,0,0,0,0};


//#define AVG_CNT_P 20
//unsigned int array_PVOLT[AVG_CNT_P];

//////////////// Setting up Proxi task////////////
void PROXI_task_setup()
{
  Serial.println("This is PROXI Task Set up.");
  //pinMode(proxi_volt_pin, INPUT);
     for(uint8_t i=0;i<AVG_CNT;i++)
          array_duty[i] = 0;              //put all array to zero of CP task initially during set up
      for(uint8_t i=0;i<AVG_CNT;i++)
          array_freq[i] = 0;
      for(uint8_t i=0;i<SLOW_CNT;i++)
          array_duty_slow[i] = 0;
}

///////////////////////////// Main Proxi Task///////////////////
static void PROXI_task(void *pvParameters)
{
  while(1)
  {
  Serial.println("************************* PROXI Task Start ********************************");
  digitalWrite(LED_BUILTIN, HIGH); // turn on Led to detect freeze of task. 

  for(uint8_t i = 0; i<AVG_PCNT ; i++)    // put analog values in prxi volt array. 
    proxi_volt[i] = analogRead(proxi_volt_pin);

  unsigned int proxi_volt_avg = 0;
  for(uint8_t i = 0; i<AVG_PCNT ; i++) // total of array
    proxi_volt_avg = proxi_volt_avg + proxi_volt[i];
  proxi_volt_avg = proxi_volt_avg/AVG_PCNT; // average by divide
  
  Serial.print("Proxi Voltage : ");
  Serial.println(proxi_volt_avg);


  //Serial.print("Duty Cycle : ");
  //Serial.println(duty_avg); 

                                  // Now detect the plug present and the proxi resistance value bw 200 and 250
  if(proxi_volt_avg < 250 )                     
    //if(proxi_volt > 200 & proxi_volt < 250 )   //--- change this when connect real charger
    {
     vTaskResume(Handle_CP_task);  // Enable CP Task when plug present
     vTaskResume(Handle_OBC_task); // Enable OBC Task when plug present
    }
  else
    {
      vTaskSuspend(Handle_CP_task); // Disable CP Task when plug present
      vTaskSuspend(Handle_OBC_task); // Disable OBC Task when plug present
      charge_start_flag = 0;     // Reset Flag
      charge_duty_error = 0;
      freq_avg = 0;
      duty_avg = 0;
      max_charge_curr = 0;

      for(uint8_t i=0;i<AVG_CNT;i++) // Reset All Array to zero
          array_duty[i] = 0;
      for(uint8_t i=0;i<AVG_CNT;i++)
          array_freq[i] = 0;
      for(uint8_t i=0;i<SLOW_CNT;i++)
          array_duty_slow[i] = 0;

      duty_time = 0;             // Reset Variable to 0
      freq_time = 0;
  
  }


digitalWrite(LED_BUILTIN, LOW);

  Serial.println("************************* PROXI Task End ********************************");

  //taskYIELD();
  myDelayMs(5000);
  }
}
