// calculate Control Pilot Duty cycle and Frequency
// If Error, set charge_duty_error and charge_start_flag

const byte interruptPin = 0;

#define AVG_CNT 20 // Averaging count 
#define SLOW_CNT 10


unsigned long last_time1, curr_time1, duty_time, freq_time;

char flag = 0;
char charge_start_flag = 0;
unsigned int avg = 0;

unsigned int array_duty[AVG_CNT]; //array for duty avg
unsigned int array_freq[AVG_CNT]; // array for freq avg
unsigned int array_duty_slow[SLOW_CNT]; // arra for slow


void CP_task_setup() // setting up task set up
{
  Serial.println("This is CP Task Setup.");
  
  for(uint8_t i=0;i<AVG_CNT;i++)
  array_duty[i] = 0;
  pinMode(interruptPin, INPUT);

}

static void CP_task(void *pvParameters) // main cp task
{
  while(1)
  {
  Serial.println("**************************************** CP Task Start ****************************************.");
  digitalWrite(LED_BUILTIN, HIGH); // turn on led to detect error
  //delay(5000);
  caluculate_duty();  // this function calculate duty cycle and freq first time, and set charge_duty_error (0 no error, 5 error)
  // set charge_start_flag one if duty and freq are fine 

  if(charge_duty_error!=5 & charge_start_flag==1) 
  {  
    duty_slow_fifo(duty_avg, SLOW_CNT);   // every 5 seconds it gets duty avg and put it in this array
    avg = 0;
    // because of 5 seconds, the duty avg will go slowly to its calculated value
    for(uint8_t i=0;i<SLOW_CNT;i++) // total array
        avg = avg + array_duty_slow[i];
        avg = avg/SLOW_CNT;  // average by divide
        max_charge_curr = (unsigned short) ((avg) * 0.6)/10; // finally calculate the maximum current from the duty cycle

  }else max_charge_curr = 0; //check this cndition



  Serial.print("Duty Cycle : ");
  Serial.println(duty_avg, DEC); 

  Serial.print("Freq Avg : ");
  Serial.println(freq_avg, DEC);

  Serial.print("Maximum Charge Current : ");
  Serial.println(max_charge_curr, DEC);  

  Serial.print("Charge Duty Error : ");
  Serial.println(charge_duty_error, DEC);

  Serial.print("Charge Start Flag : ");
  Serial.println(charge_start_flag, DEC);


  digitalWrite(LED_BUILTIN, LOW); // turn off led 

  Serial.println("**************************************** CP Task End *****************************************.");
  //taskYIELD(); // cooperative scheduling
  myDelayMs(5000); // suspend task for 5 seconds
  }
}


void caluculate_duty()
{
    if(charge_start_flag==0) // if 0 
    {
      attachInterrupt(digitalPinToInterrupt(interruptPin), control_pilot, CHANGE); //turn on interrupt at interrupt pin and each intterupt call control_pilot func
      delay(1000); // during this period duty will be calculated
      detachInterrupt(digitalPinToInterrupt(interruptPin)); //after one second, interrupt will be disable
      // now both avg arrays are full of values
        avg=0;
        for(uint8_t i=0;i<AVG_CNT;i++) // total whole avg
          avg = avg + array_duty[i];
        avg = avg/AVG_CNT;            // avgerage by divide
        duty_avg = avg+6;             // add offset 6

        avg=0;
        for(uint8_t i=0;i<AVG_CNT;i++) // total whole abg
        avg = avg + array_freq[i];
        avg = avg/AVG_CNT;          // avgerage by divide
        freq_avg = avg;


      if((duty_avg >= 100 & duty_avg <=960) & (freq_avg >800 & freq_avg <1200) ) // if avg bw 10 to 96 and freq bw 800 to 1200
      {
        //max_charge_curr = (unsigned short) ((duty_avg) * 0.6)/10;
        charge_start_flag = 1;                                               // duty and freq is in limit and fine
        charge_duty_error = 0;                                               // no duty error
      }
      else
      { 
        charge_duty_error = 5;                                          // error if avg and freq fails to meet clriteria
        max_charge_curr = 0;
      }

    }
    
    if(charge_start_flag) // once freq and duty fine in first step, keep calculating duty cycle to calculate current in real time
    {
      attachInterrupt(digitalPinToInterrupt(interruptPin), control_pilot, CHANGE); 
      delay(1000);
      detachInterrupt(digitalPinToInterrupt(interruptPin));
      
        unsigned int avg=0;
        for(uint8_t i=0;i<AVG_CNT;i++) //total whole array
          avg = avg + array_duty[i];
        avg = avg/AVG_CNT;              // average by divide
        duty_avg = avg+6;

        if(duty_avg >= 100 & duty_avg <=960)
        {
           //max_charge_curr = (unsigned short) ((duty_avg) * 0.6)/10; // caluculate the current
           charge_duty_error = 0;
        }
        else
        charge_duty_error++;
        if(charge_duty_error>5)charge_duty_error=5;
    }
}


void control_pilot() // this is called during change on interrupt
{
//digitalWrite(LED_BUILTIN, HIGH);

  if(digitalRead(interruptPin)) // pin 1
  {
      if(!flag) // if flag 0
      {
        freq_time = micros() - last_time1; // current freq
        freq_fifo (freq_time, AVG_CNT); // put it in array of multiple freq to calculate avg
      }

      flag = 1;
      last_time1 = micros(); //
  
  }
  else if(flag) // if flag 1 and pin 0
  {
      flag = 0; 
      curr_time1 = micros(); 
      duty_time = curr_time1-last_time1; //calaylute duty 
      //diff_time = diff_time/10;
      duty_fifo (duty_time, AVG_CNT); // put it in array of multiple duty to calculate avduty cycg
  }
}

void duty_fifo (unsigned int val, uint8_t avgn) // fill up fifo of the duty cycle 
{
 for(uint8_t i=0;i<avgn;i++) //shift all to left
 array_duty[i] = array_duty[i+1];
 array_duty[avgn-1]=val; //set last value 
}

void freq_fifo (unsigned int val, uint8_t avgn) // fill up fifo of the cp freq
{
 for(uint8_t i=0;i<avgn;i++) //shift all to left
 array_freq[i] = array_freq[i+1];
 array_freq[avgn-1]=val; //set last value 
}

void duty_slow_fifo (unsigned int val, uint8_t avgn) // fill up fifo of the cp freq
{
 for(uint8_t i=0;i<avgn;i++) //shift all to left
 array_duty_slow[i] = array_duty_slow[i+1];
 array_duty_slow[avgn-1]=val; //set last value 
}
