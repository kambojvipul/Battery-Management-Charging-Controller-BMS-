// Read and Send OBC CAN Messages using CAN2

#define COMM_TIMEOUT 2000 //this is timeout for reading a CAN Message

unsigned int last_time;

void OBC_task_setup()
{
  Serial.println("This is OBC Task Setup.");
}

// This is Main OBC Task ///////
static void OBC_task(void *pvParameters)
{
  while(1)
  {
  Serial.println("******************************************************** OBC Task Start ************************************************************");

digitalWrite(LED_BUILTIN, HIGH); // Turn Led ON to detect freeze

  Serial.println("OBC Sending packet ... ");

  charge_control(); // Decide Charge Current and Voltage based on SoC, Temp, AC Voltage, Max Charge Current
  charge_command(ch_volt_cmd, ch_curr_cmd, charge_cmd); //Volt: 49.9 and Current 49.9 Amp and start_ch (0=start charging, 1 = charger off, 2 = heating start) 
  get_charge_status();

  Serial.print("Charge Volt Fb :"); Serial.println(ch_volt_fb);
  Serial.print("Charge Current Fb :"); Serial.println(ch_curr_fb);
  Serial.print("Charge COMM Error :"); Serial.println(obc_com_rxerr,HEX);

digitalWrite(LED_BUILTIN, LOW); // Turn Led OFF to detect freeze

  Serial.println("******************************************************** OBC Task END ************************************************************");

//taskYIELD();
  myDelayMs(5000); // Suspend Task for 5 Seconds
  }
}


////////////////////////////////////////////////////////////////////////////////// External Function ///////////////////////////////////////////////////////////////////////

void charge_command(uint16_t charge_volt, uint16_t charge_curr, uint8_t start_ch) // Tx Charge Voltage, Charge Current and Start Charging Signal, CAN ID: 0x1806E5FF
{
  uint8_t charge_volt_h, charge_volt_l, charge_curr_h, charge_curr_l;

  charge_volt_h = ((uint16_t)charge_volt >> 8) & 0xFF;
  charge_volt_l = ((uint16_t)charge_volt >> 0) & 0xFF;

  charge_curr_h = ((uint16_t)charge_curr >> 8) & 0xFF;
  charge_curr_l = ((uint16_t)charge_curr >> 0) & 0xFF;

  CAN2.beginExtendedPacket(0x1806E5FF,8,0);
  CAN2.write(charge_volt_h); //byte 1
  CAN2.write(charge_volt_l); //byte 2
  CAN2.write(charge_curr_h); //byte 3
  CAN2.write(charge_curr_l); //byte 4
  CAN2.write(start_ch); //byte 5
  CAN2.write(0x0);
  CAN2.write(0x0);
  CAN2.write(0x0);
  CAN2.endPacket();
}

void get_charge_status() // Rx Charge FB Voltage and Current, OBC DTO Code if any, CAN ID: 0x18FF50E5
{
  char ch_volt_h, ch_volt_l, ch_curr_h, ch_curr_l;

  last_time = millis();

    while(1) 
    {
      // try to parse packet
      char packetSize = CAN2.parsePacket();

        ////////////////Break Loop if time out////////////////
        if( (millis() -last_time) > COMM_TIMEOUT){ obc_com_rxerr = 1;  Serial.println("OBC RX FAILED."); break;}
        else obc_com_rxerr = 0;
        /////////////////////////////////

      if (packetSize || CAN2.packetId() != -1) 
      {
        // received a packet
        Serial.println("Received ");
        Serial.print("packet with id 0x");
        uint can_id = CAN2.packetId();
        Serial.println(can_id, HEX);    
    
        if (!CAN2.packetRtr() & CAN2.packetExtended() & (can_id == 0x18FF50E5)) // 
        {
          //Serial.print(" and length ");
          //Serial.println(packetSize);
          // only print packet data for non-RTR packets

    
          char i = 0;
          while (CAN2.available()) 
          {
            char b_data = CAN2.read(); 
            //Serial.print(b_data, HEX);
            switch(i)
            {
              case 0: ch_volt_h = b_data; break;
              case 1: ch_volt_l = b_data; break;
              case 2: ch_curr_h = b_data; break;
              case 3: ch_curr_l = b_data; break;
              case 4: OBC_dto_code = b_data; break;
              default: break;
            }
            i++;
          }
          ch_volt_fb = (ch_volt_h * 256 + ch_volt_l)/10;
          ch_curr_fb = (ch_curr_h * 256 + ch_curr_l)/10;
          
          break;
        }
      }
    }
}

