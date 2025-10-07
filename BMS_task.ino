// Read and Send BMS CAN Messages using CAN

#include <CAN.h>

//unsigned int last_time;
unsigned int last_time2 = 0;
#define COMM_TIMEOUT1 1000  //this is timeout for reading a CAN Message
//first message
uint8_t volt_h, volt_l, curr_h, curr_l, energy_h, energy_l;  // voltage, current and energy: high low variables

//second message
uint8_t AH_h, AH_l, dis_clc_h, dis_clc_l, soc_h, soc_l;  // Current, discharge cycle, SOC: high low variables

//third message

//fifth message
uint8_t NTC1_h, NTC1_l, NTC2_h, NTC2_l, NTC3_h, NTC3_l, NTC4_h, NTC4_l, NTC5_h, NTC5_l;  // NTC1 to 5: low high variables


//cell_balancing
uint8_t cell_bal_h, cell_bal_l;  // cell balancing: low high variables


//cell voltage
uint8_t cell_volt_h[16], cell_volt_l[16];  //array of Individual Cell Voltage





void BMS_task_setup() {
  Serial.println("This is BMS Task Setup.");
}

///////////////// BMS Main Task ///////////////////
static void BMS_task(void *pvParameters) {
  while (1) {
    Serial.println("***********************************************BMS Task Start ************************************************");
    digitalWrite(LED_BUILTIN, HIGH); // turn ON led to detect freeze dusing BMS Task////////
    Serial.println("Bat Sending packet ... ");


    msg_100(); // read Message CAN ID 0x100 to 0x106 below
    msg_101();
    msg_102();
                  Watchdog.reset();
    msg_103();
    msg_105();
    msg_106();

    read_cell_voltage(0x107, 0);  //read cell voltage from 0 to 2 from Message ID: 0x107 
    read_cell_voltage(0x108, 3);  //read cell voltage from 0 to 3 to 5 from Message ID: 0x108
    read_cell_voltage(0x109, 6);  //read cell voltage from 0 to 6 to 8 from Message ID: 0x109
    Watchdog.reset();
    read_cell_voltage(0x10A, 9);   //read cell voltage from 9 to 11 from Message ID: 0x10A
    read_cell_voltage(0x10B, 12);  //read cell voltage from 12 to 14 from Message ID: 0x10B

    bat_minTemp = find_minTemp(); // Battery Minimum Temperature
    bat_maxTemp = find_maxTemp(); // Battery Maximum Temperature

    /////////////////////////// Display /////////////////////////////////////

    Serial.println("Received Message");
    Serial.print("Voltage :");
    Serial.println(bat_volt / 100);
    Serial.print("Current :");
    Serial.println(bat_current / 100);
    Serial.print("Energy :");
    Serial.println(bat_energy / 100);

    Serial.print("Capacity_AH :");
    Serial.println(bat_capacity_ah / 100);
    Serial.print("Dis_Cycle :");
    Serial.println(bat_dis_cycle);
    Serial.print("Battery SOC :");
    Serial.println(bat_soc);

    Serial.print("Cell Balancing :");
    Serial.println(cell_bal);
    Serial.print("Connection :");
    Serial.println(bat_connection, HEX);

    Serial.print("NTC1 :");
    Serial.println(NTC1);
    Serial.print("NTC2 :");
    Serial.println(NTC2);
    Serial.print("NTC3 :");
    Serial.println(NTC3);

    Serial.print("NTC4 :");
    Serial.println(NTC1);
    Serial.print("NTC5 :");
    Serial.println(NTC2);



    for (char x = 0; x < 15; x++) 
    {
      Serial.print("Cell Voltage ");
      Serial.print(x, HEX);
      Serial.print(" :");
      Serial.println(cell_volt[x]);
    }
    Serial.print("BMS_rxerr : ");
    Serial.println(BMS_rxerr, BIN);
    
    digitalWrite(LED_BUILTIN, LOW); //put board led off

    Serial.println("**************************************************BAT TASK END ************************************************");

    //taskYIELD();
    myDelayMs(5000); // Suspend Task for 5 seconds
  } 
}

//////////////////////////////////External Functions//////////////////////////////////////////////////////
void read_cell_voltage(int msg_id, char cell_id) {
  CAN.beginPacket(msg_id, 8, 1);
  CAN.endPacket();
  //Receive

  last_time2 = millis();

  while (1) {
    int can_id = 0;
    // try to parse packet
    char packetSize = CAN.parsePacket();


    ////////////////Break Loop if time out////////////////
    if (cell_id == 0) {
      if ((millis() - last_time2) > COMM_TIMEOUT1) {
        BMS_rxerr = BMS_rxerr | 0x0040;
        Serial.print("cell_id ");
        Serial.print(cell_id, DEC);
        Serial.println(" RX FAILED.");
        break;
      } else BMS_rxerr = BMS_rxerr & 0xFFBF;
    } else if (cell_id == 3) {
      if ((millis() - last_time2) > COMM_TIMEOUT1) {
        BMS_rxerr = BMS_rxerr | 0x0080;
        Serial.print("cell_id ");
        Serial.print(cell_id, DEC);
        Serial.println(" RX FAILED.");
        break;
      } else BMS_rxerr = BMS_rxerr & 0xFF7F;
    } else if (cell_id == 6) {
      if ((millis() - last_time2) > COMM_TIMEOUT1) {
        BMS_rxerr = BMS_rxerr | 0x0100;
        Serial.print("cell_id ");
        Serial.print(cell_id, DEC);
        Serial.println(" RX FAILED.");
        break;
      } else BMS_rxerr = BMS_rxerr & 0xFEFF;
    } else if (cell_id == 9) {
      if ((millis() - last_time2) > COMM_TIMEOUT1) {
        BMS_rxerr = BMS_rxerr | 0x0200;
        Serial.print("cell_id ");
        Serial.print(cell_id, DEC);
        Serial.println(" RX FAILED.");
        break;
      } else BMS_rxerr = BMS_rxerr & 0xFDFF;
    } else if (cell_id == 12) {
      if ((millis() - last_time2) > COMM_TIMEOUT1) {
        BMS_rxerr = BMS_rxerr | 0x0400;
        Serial.print("cell_id ");
        Serial.print(cell_id, DEC);
        Serial.println(" RX FAILED.");
        break;
      } else BMS_rxerr = BMS_rxerr & 0xFBFF;
    }

    /////////////////////////////////

    if (packetSize || CAN.packetId() != -1) {
      // received a packet
      Serial.println("Received ");

      Serial.print("packet with id 0x");
      can_id = CAN.packetId();
      Serial.print(can_id, HEX);

      if (!CAN.packetRtr() & (can_id == msg_id)) {

        char i = 0;
        while (CAN.available()) {
          char b_data = CAN.read();
          //Serial.print(b_data, HEX);
          switch (i) {
            case 0: cell_volt_h[cell_id] = b_data; break;
            case 1: cell_volt_l[cell_id] = b_data; break;
            case 2: cell_volt_h[cell_id + 1] = b_data; break;
            case 3: cell_volt_l[cell_id + 1] = b_data; break;
            case 4: cell_volt_h[cell_id + 2] = b_data; break;
            case 5: cell_volt_l[cell_id + 2] = b_data; break;
            default: break;
          }
          i++;
        }
        cell_volt[cell_id] = cell_volt_h[cell_id] * 256 + cell_volt_l[cell_id];
        cell_volt[cell_id + 1] = cell_volt_h[cell_id + 1] * 256 + cell_volt_l[cell_id + 1];
        cell_volt[cell_id + 2] = cell_volt_h[cell_id + 2] * 256 + cell_volt_l[cell_id + 2];
        break;
      }
    }
  }
}

void msg_100() // Reading CAN Messahe ID: 0x100
{
  //First Message
  CAN.beginPacket(0x100, 8, 1);
  CAN.endPacket();
  ///////////////////////////////////////////////////////////////////////////////////
  last_time2 = millis();
  while (1) {
    // try to parse packet
    char packetSize = CAN.parsePacket();

    ////////////////Break Loop if time out////////////////
    if ((millis() - last_time2) > COMM_TIMEOUT1) {
      BMS_rxerr = BMS_rxerr | 0x0001;
      Serial.println("MSG100 RX FAILED.");
      break;
    } else BMS_rxerr = BMS_rxerr & 0xFFFE;
    /////////////////////////////////

    if (packetSize || CAN.packetId() != -1) {
      // received a packet
      Serial.println("Received ");

      Serial.print("packet with id 0x");
      int can_id = CAN.packetId();
      Serial.print(can_id, HEX);

      if (!CAN.packetRtr() && (can_id == 0x100))  // 0x100
      {
        Serial.println("Loop Inside");
        char i = 0;
        while (CAN.available()) {
          char b_data = CAN.read();
          //Serial.print(b_data, HEX);
          switch (i) {
            case 0: volt_h = b_data; break;
            case 1: volt_l = b_data; break;
            case 2: curr_h = b_data; break;
            case 3: curr_l = b_data; break;
            case 4: energy_h = b_data; break;
            case 5: energy_l = b_data; break;
            default: break;
          }
          i++;
        }
        bat_volt = volt_h * 256 + volt_l;
        bat_current = curr_h * 256 + curr_l;
        bat_energy = energy_h * 256 + energy_l;
        break;
      }
    }
  }
}

void msg_101() // Reading CAN Messahe ID: 0x101
{
  //Second Message
  CAN.beginPacket(0x101, 8, 1);
  CAN.endPacket();
  //Receive

  last_time2 = millis();



  while (1) {
    // try to parse packet
    char packetSize = CAN.parsePacket();

    ////////////////Break Loop if time out////////////////
    if ((millis() - last_time2) > COMM_TIMEOUT1) {
      BMS_rxerr = BMS_rxerr | 0x0002;
      Serial.println("MSG101 RX FAILED.");
      break;
    } else BMS_rxerr = BMS_rxerr & 0xFFFD;
    /////////////////////////////////

    if (packetSize || CAN.packetId() != -1) {
      // received a packet
      Serial.println("Received ");

      Serial.print("packet with id 0x");
      int can_id = CAN.packetId();
      Serial.print(can_id, HEX);

      if (!CAN.packetRtr() & (can_id == 0x101)) {

        char i = 0;
        while (CAN.available()) {
          char b_data = CAN.read();
          //Serial.print(b_data, HEX);
          switch (i) {
            case 0: AH_h = b_data; break;
            case 1: AH_l = b_data; break;
            case 2: dis_clc_h = b_data; break;
            case 3: dis_clc_l = b_data; break;
            case 4: soc_h = b_data; break;
            case 5: soc_l = b_data; break;
            default: break;
          }
          i++;
        }
        bat_capacity_ah = AH_h * 256 + AH_l;
        bat_dis_cycle = dis_clc_h * 256 + dis_clc_l;
        bat_soc = soc_h * 256 + soc_l;
        break;
      }
    }
  }
}

void msg_102() // Reading CAN Messahe ID: 0x102
{
  //Second Message
  CAN.beginPacket(0x102, 8, 1);
  CAN.endPacket();
  ///////////////////////////////////////////////////////////////////////////////////
  last_time2 = millis();

  while (1) {
    // try to parse packet
    char packetSize = CAN.parsePacket();


    ////////////////Break Loop if time out////////////////
    if ((millis() - last_time2) > COMM_TIMEOUT1) {
      BMS_rxerr = BMS_rxerr | 0x0004;
      Serial.println("MSG102 RX FAILED.");
      break;
    } else BMS_rxerr = BMS_rxerr & 0xFFFB;
    /////////////////////////////////

    if (packetSize || CAN.packetId() != -1) {
      // received a packet
      Serial.println("Received ");

      Serial.print("packet with id 0x");
      int can_id = CAN.packetId();
      Serial.print(can_id, HEX);

      if (!CAN.packetRtr() & (can_id == 0x102)) {

        char i = 0;
        while (CAN.available()) {
          char b_data = CAN.read();
          //Serial.print(b_data, HEX);
          switch (i) {

            case 0: cell_bal_h = b_data; break;
            case 1: cell_bal_l = b_data; break;

            default: break;
          }
          i++;
        }
        cell_bal = cell_bal_h * 256 + cell_bal_l;
        break;
      }
    }
  }
}

void msg_103() // Reading CAN Messahe ID: 0x103
{
  //Third Message
  CAN.beginPacket(0x103, 8, 1);
  CAN.endPacket();
  ///////////////////////////////////////////////////////////////////////////////////
  last_time2 = millis();
  while (1) {
    // try to parse packet
    char packetSize = CAN.parsePacket();

    ////////////////Break Loop if time out////////////////
    if ((millis() - last_time2) > COMM_TIMEOUT1) {
      BMS_rxerr = BMS_rxerr | 0x0008;
      Serial.println("MSG103 RX FAILED.");
      break;
    } else BMS_rxerr = BMS_rxerr & 0xFFF7;
    /////////////////////////////////

    if (packetSize || CAN.packetId() != -1) {
      // received a packet
      Serial.println("Received ");

      Serial.print("packet with id 0x");
      int can_id = CAN.packetId();
      Serial.print(can_id, HEX);

      if (!CAN.packetRtr() & (can_id == 0x103)) {

        char i = 0;
        while (CAN.available()) {
          char b_data = CAN.read();
          //Serial.print(b_data, HEX);
          switch (i) {

            case 1: bat_connection = b_data; break;

            default: break;
          }
          i++;
        }

        break;
      }
    }
  }
}

void msg_105() // Reading CAN Messahe ID: 0x105
{
  //Fifth Message
  CAN.beginPacket(0x105, 8, 1);
  CAN.endPacket();
  ///////////////////////////////////////////////////////////////////////////////////
  last_time2 = millis();

  while (1) {
    // try to parse packet
    char packetSize = CAN.parsePacket();

    ////////////////Break Loop if time out////////////////
    if ((millis() - last_time2) > COMM_TIMEOUT1) {
      BMS_rxerr = BMS_rxerr | 0x0010;
      Serial.println("MSG105 RX FAILED.");
      break;
    } else BMS_rxerr = BMS_rxerr & 0xFFEF;
    /////////////////////////////////

    if (packetSize || CAN.packetId() != -1) {
      // received a packet
      Serial.println("Received ");

      Serial.print("packet with id 0x");
      int can_id = CAN.packetId();
      Serial.print(can_id, HEX);

      if (!CAN.packetRtr() & (can_id == 0x105))  // 0x100
      {

        char i = 0;
        while (CAN.available()) {
          char b_data = CAN.read();
          //Serial.print(b_data, HEX);
          switch (i) {
            case 0: NTC1_h = b_data; break;
            case 1: NTC1_l = b_data; break;
            case 2: NTC2_h = b_data; break;
            case 3: NTC2_l = b_data; break;
            case 4: NTC3_h = b_data; break;
            case 5: NTC3_l = b_data; break;
            default: break;
          }
          i++;
        }
        NTC1 = (NTC1_h * 256 + NTC1_l) - 2731;
        NTC2 = (NTC2_h * 256 + NTC2_l) - 2731;
        NTC3 = (NTC3_h * 256 + NTC3_l) - 2731;
        break;
      }
    }
  }
}

void msg_106() // Reading CAN Messahe ID: 0x106
{
  CAN.beginPacket(0x106, 6, 1);
  CAN.endPacket();
  ///////////////////////////////////////////////////////////////////////////////////
  last_time2 = millis();

  while (1) {
    // try to parse packet
    char packetSize = CAN.parsePacket();


    ////////////////Break Loop if time out////////////////
    if ((millis() - last_time2) > COMM_TIMEOUT1) {
      BMS_rxerr = BMS_rxerr | 0x0020;
      Serial.println("MSG106 RX FAILED.");
      break;
    } else BMS_rxerr = BMS_rxerr & 0xFFDF;
    /////////////////////////////////

    if (packetSize || CAN.packetId() != -1) {
      // received a packet
      Serial.println("Received ");

      Serial.print("packet with id 0x");
      int can_id = CAN.packetId();
      Serial.print(can_id, HEX);

      if (!CAN.packetRtr() & (can_id == 0x106))  // 0x100
      {

        char i = 0;
        while (CAN.available()) {
          char b_data = CAN.read();
          //Serial.print(b_data, HEX);
          switch (i) {
            case 0: NTC4_h = b_data; break;
            case 1: NTC4_l = b_data; break;
            case 2: NTC5_h = b_data; break;
            case 3: NTC5_l = b_data; break;

            default: break;
          }
          i++;
        }
        NTC4 = (NTC1_h * 256 + NTC1_l) - 2731;
        NTC5 = (NTC2_h * 256 + NTC2_l) - 2731;
        break;
      }
    }
  }
}



int16_t find_minTemp() // Find Minimum temperature
{
  int16_t temp = min(NTC1, NTC2);
  temp = min(temp, NTC3);
  temp = min(temp, NTC4);
  return min(temp, NTC5);
}

int16_t find_maxTemp() // Find Maximum temperature
{
  int16_t temp = max(NTC1, NTC2);
  temp = max(temp, NTC3);
  temp = max(temp, NTC4);
  return max(temp, NTC5);
}
