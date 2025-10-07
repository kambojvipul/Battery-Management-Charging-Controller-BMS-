/*
  Pins Distribution:

  Charging Inlet Pins:
    Pin A1: Proximity Analog Pin
    Pin 0: Control Pilot Pin
    Pin A2: Start_ch_pin
    Pin A6: Ch_sw_pin

  Serial1 Pins connection to ESP32C3:
    Pin 13: UART RX
    PIN 14: UART TX 

  CAN 500Kbps BMS
    Pin 3: CS or SS
    Pin 10: MISO
    Pin 8: MOSI
    Pin 9: SCK
    Pin 7: INT

  CAN2 250Kbps OBC
    Pin 4: CS or SS
    Pin 10: MISO
    Pin 8: MOSI
    Pin 9: SCK
    Pin 7: INT

  Switch Pins:
    Pin 5: 12 Volt Switch
    Pin 6: Inverter Switch
    Pin A0: Solar Charge Switch
    Pin A2
  
  Led Lights Pins:
    Pin 1: Greed Led
    Pin 2: Red Led

  Temperature Pins:
    Pin A3 : Control Box/Inverter Temp Sensor
    Pin A4 : OBC Temp Sensor

  AC Input Voltage:
    Pin A5 : Input AC Voltage

  I2C Led Display Output:
    Pin 11: SDA
    Pin 12: SCL
*/


/*
// Battery 

uint16_t bat_volt, bat_current, bat_energy;
uint16_t bat_capacity_ah, bat_dis_cycle, bat_soc;
char bat_connection;
uint16_t NTC1, NTC2, NTC3, NTC4, NTC5;
uint16_t bat_minTemp, bat_maxTemp;
uint16_t cell_bal;
uint16_t cell_volt[16];

//OBC

char obc_com_rxerr = 0;
uint16_t ch_volt_cmd, ch_curr_cmd = 0;
uint16_t ch_volt_fb, ch_curr_fb = 0;
uint8_t charge_cmd = 0;
uint8_t OBC_dto_code = 0;

// CP and Proxi

short max_charge_curr = 0;
unsigned int diff_avg;
char charge_duty_error = 0;

*/