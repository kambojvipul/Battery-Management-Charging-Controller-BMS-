#define DC_SW 5
#define INV_SW 6
#define SOLAR_CHARGE_PIN A0
#define CHARGE_RELAY A6
#define GLED_PIN 1
#define RLED_PIN 2
#define CONTB_TEMP_PIN A3
#define OBC_TEMP_PIN A4
#define IN_AC_PIN A5

void IO_task_setup()
{
  Serial.println("This is IO Task Setup.");

  pinMode(DC_SW, OUTPUT); // 12 Volt Switch
  digitalWrite(DC_SW, LOW); 

  pinMode(INV_SW, OUTPUT); // Inverter Switch
  digitalWrite(INV_SW, LOW);

  pinMode(SOLAR_CHARGE_PIN, OUTPUT); // Solar Charge Switch
  digitalWrite(SOLAR_CHARGE_PIN, LOW);

  pinMode(CHARGE_RELAY, OUTPUT); // Charge Switch
  digitalWrite(CHARGE_RELAY, LOW);

  pinMode(GLED_PIN, OUTPUT); // Green Led
  digitalWrite(GLED_PIN, LOW);

  pinMode(RLED_PIN, OUTPUT); // Red Led
  digitalWrite(RLED_PIN, LOW);

  pinMode(CONTB_TEMP_PIN, INPUT); // Control Box/Inverter Temp Sensor

  pinMode(OBC_TEMP_PIN, INPUT); // OBC Temp Sensor

  pinMode(IN_AC_PIN, INPUT); // Input AC Voltage

}

static void IO_task(void *pvParameters)
{
  while(1)
  {
      Serial.println("*******************************************************IO Task Start ************************************************.");
      digitalWrite(LED_BUILTIN, HIGH); // turn on Led to detect freeze of task. 
       dc_switch_control();
       inv_switch_control();
       control_solar_charge();
       control_ch_switch();
       green_ch_led();
       red_err_led();
       read_temp();
       read_VAC();
       
        digitalWrite(LED_BUILTIN, LOW); // turn on Led to detect freeze of task. 
       myDelayMs(1000);
      Serial.println("*******************************************************IO Task END ************************************************.");
  }
}

void dc_switch_control()
{
  if(dc12v_switch)
    digitalWrite(DC_SW, HIGH);
  else
    digitalWrite(DC_SW, LOW); 
}

void inv_switch_control()
{
  if(inv_switch)
  digitalWrite(INV_SW, HIGH);
  else
  digitalWrite(INV_SW, LOW);
}

void control_solar_charge()
{
  if(bat_soc<=100 && charge_start_flag == 0)
  {
    digitalWrite(SOLAR_CHARGE_PIN, HIGH);
  }
  else
  {
    digitalWrite(SOLAR_CHARGE_PIN, LOW);
  }
}

void control_ch_switch()
{
  if(charge_start_flag)
    digitalWrite(CHARGE_RELAY, HIGH);
  else
    digitalWrite(CHARGE_RELAY, LOW);

}

void green_ch_led()
{
  if(charge_start_flag)
  {
    digitalWrite(GLED_PIN, LOW);
    myDelayMs((100-bat_soc)*10); 
    digitalWrite(GLED_PIN, HIGH);
  }

}

void red_err_led()
{
  if(charge_duty_error == 5)
  {
      digitalWrite(RLED_PIN, HIGH);
      myDelayMs(50);
      digitalWrite(RLED_PIN, LOW);  
      myDelayMs(50);
      digitalWrite(RLED_PIN, HIGH);  
      myDelayMs(50);
      digitalWrite(RLED_PIN, LOW);  
      myDelayMs(50);
      digitalWrite(RLED_PIN, HIGH);  
  }
  else
      digitalWrite(RLED_PIN, LOW);

  if(ch_comb_err)
      digitalWrite(RLED_PIN, HIGH);
  else
      digitalWrite(RLED_PIN, LOW);
}

void read_temp()
{
  box_temp = analogRead(CONTB_TEMP_PIN);
  charge_temp = analogRead(OBC_TEMP_PIN);  
}

void read_VAC()
{
  int raw_VAC = analogRead(IN_AC_PIN);  
}