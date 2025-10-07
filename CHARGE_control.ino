// Calculate the Maximum Charge Current that Batery can support, Based on SoC and Temperature using 2D Matrix, Standard used Inventus
// Calculate Available Max Current based on Input AC Voltage and Control Pilot Duty Cycle (current)
// Before commanding Final Voltage and Current Value: check Over Current, Voltage Error from FB from OBC, Max/Min Battery Temperature Threshold 
// If Battery is full, SoC: 100%, the OBC is off. 

#include <stdio.h>
/////////// Charge Current Value in Matrix: X Axis: SoC and Y Axis: Temperature ////////////
int chargeTable[15][21] = 
{
  //{0, 1, 2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},// Index Steps in SoC
//{100,99,94, 89, 84, 79, 74, 69, 64, 59, 54, 49, 44, 39, 34, 29, 24, 19, 14,  9,  5},// SoC in X Axis : mapToSoCIndex Function
    {0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},//0 - Temp: 60-100--- in Y Axis
    {0, 4, 9,  9,  9,  9,  9,  9,  9,  9,  9, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18},//1 - Temp: 55-60////
    {0, 6,12, 12, 12, 12, 12, 12, 12, 12, 12, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},//2 - Temp: 50-55----
    {0, 9,18, 18, 18, 18, 18, 18, 18, 18, 18, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45},//3 - Temp: 45-50////
    {0,12,21, 30, 30, 30, 30, 30, 30, 30, 30, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67},//4 - Temp: 40-45----
    {0,15,25, 45, 45, 45, 45, 45, 45, 45, 45, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90},//5 - Temp: 35-40////
    {0,15,25, 45, 45, 67, 67, 67, 67, 67, 67, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90},//6 - Temp: 30-35----
    {0,15,25, 45, 45, 67, 67, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90},//7 - Temp: 25-30////
    {0,15,25, 45, 45, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90},//8 - Temp: 20-25----
    {0, 4, 9, 18, 25, 45, 45, 67, 67, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90},//9 - Temp: 15-20////
    {0, 4, 5,  9, 18, 45, 45, 45, 45, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90},//10 -Temp: 10-15----
    {0, 0, 0,  0,  9, 18, 18, 36, 36, 54, 54, 54, 54, 72, 72, 72, 72, 72, 72, 72, 72},//11 - Temp: 5-10////
    {0, 0, 0,  0,  0,  9,  9, 18, 18, 18, 36, 36, 36, 54, 54, 54, 54, 54, 54, 54, 54},//12 - Temp: 0-5-----
    {0, 0, 0,  0,  0,  0,  5,  9,  9,  9, 18, 18, 18, 36, 36, 36, 36, 36, 36, 36, 36},//13 - Temp: 5-0/////
    {0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} //14 -Temp: 10-5----- in Y Axis
  //                                                                                 //mapToTempIndex Function
 };

// Helper function to map SOC and temperature to table indices
// Based on SoC Value, it gives X Axis: Index of Charge Table
int mapToSOCIndex(int soc) 
{
    if (soc >= 100) return 0;
    else if (soc >= 95) return 1;
    else if (soc >= 90) return 2;
    else if (soc >= 85) return 3;
    else if (soc >= 80) return 4;
    else if (soc >= 75) return 5;
    else if (soc >= 70) return 6;
    else if (soc >= 65) return 7;
    else if (soc >= 60) return 8;
    else if (soc >= 55) return 9;
    else if (soc >= 50) return 10;
    else if (soc >= 45) return 11;
    else if (soc >= 40) return 12;
    
    return 12;
}
// Based on Temp Value, it gives Y Axis: Index of Charge Table
int mapToTempIndex(int temp) 
{
         if (temp >= 60) return 0;
    else if (temp >= 55) return 1;
    else if (temp >= 50) return 2;
    else if (temp >= 45) return 3;
    else if (temp >= 40) return 4;
    else if (temp >= 35) return 5;
    else if (temp >= 30) return 6;
    else if (temp >= 25) return 7;
    else if (temp >= 20) return 8;
    else if (temp >= 15) return 9;
    else if (temp >= 10) return 10;
    else if (temp >= 5)  return 11;
    else if (temp >= 0)  return 12;
    else if (temp >= -5) return 13;
    else if (temp >= -10) return 14;
    
    return 14;
}


int getBatMaxChargeCurrent(int soc, int temp)  // Function to get final maximum charge current based on Charge Table
{
    int socIndex = mapToSOCIndex(soc);
    //printf("socIndex: %d \n", socIndex);
    int tempIndex = mapToTempIndex(temp);
    //printf("tempIndex: %d \n", tempIndex);
    return chargeTable[tempIndex][socIndex];
}


uint16_t concatenate_error ()
{
  uint16_t t_err = 0;
  if(charge_duty_error==5) t_err |= (1 << 0); //set charge_duty_error at position 0
  if(over_curr_err) t_err |= (1 << 1); // set over_curr_err at position 1
  if(over_volt_err) t_err |= (1 << 2); // set over_volt_err at position 2
  if(over_temp_err) t_err |= (1 << 3); // set over_temp_err at position 3
  if(under_temp_err) t_err |= (1 << 4); // set under_temp_err at position 5
  if(obc_com_rxerr) t_err |= (1 << 5); // set obc_com_rxerr at position 5
  return ((t_err<< 8) | OBC_dto_code);
}

int get_ac_voltage()
{
 return 120;
}

void charge_control()  // Main Function to Calculate Maximum Charging Current based on Charge Table of SoC & Temperature 
{
    int soc, temp;
    Serial.print("Enter SOC (0-100): ");
    Serial.println(soc);
    Serial.println("Enter Temperature (°C): ");
    Serial.println(temp);

    int maxBatChargeCurrent = getBatMaxChargeCurrent(soc, temp); // Calculate the Max Charge current of Battery from charge Table
    
    Serial.print("Maximum Charge Current: ");  
    Serial.println(maxBatChargeCurrent);
    
    int AC_voltage = get_ac_voltage(); //This function caluclate the Input AC Voltage 120VAC or 240 VAC

    // Calculate Maximum DC Charge Current based on AC Voltage Input and Control Pilot Duty Cycle
    // calculate the current based on 80% AC Load: AC: V x I = DC: V x I
    int temp_charge_curr = 0.80*(AC_voltage * max_charge_curr / charge_volt);   
    
    if(temp_charge_curr >= maxBatChargeCurrent) // Charge Current (based on AC Volt and CP Duty Current) <= maximum charge current of Battery
        temp_charge_curr = maxBatChargeCurrent;
    

    if(ch_curr_fb > temp_charge_curr*1.1) over_curr_err = 1; else over_curr_err = 0; //set over current error, if FB Ch Current > 1.1* Command Current

    if(ch_volt_fb > charge_volt*1.1) over_volt_err = 1; else over_volt_err = 0; //set over voltage error, if FB Ch Voltage > 1.1* Command Voltage
    
    if(bat_maxTemp > MAX_TEMP) over_temp_err = 1; else over_temp_err = 0; // set over temp error, if bat_maxTemp is more than Threshold Set
    
    if(bat_minTemp < MIN_TEMP) under_temp_err = 1; else under_temp_err = 0; // set under temp error, if bat_minTemp is more than Threshold Set

    ch_comb_err = concatenate_error(); // Concate all errors in One Stream DTO Code: 0: no error, other than 0: Error
    //set final charge current to OBC if no error or zero;
    if(ch_comb_err || bat_soc== 100) // If any error, all command voltage and current: 0, and if Battery is full, SoC: 100%,, OBC off
    {
      ch_curr_cmd = 0; // if combined error has any value, charge current is zero
      ch_volt_cmd = 0;
      charge_cmd = OBC_OFF; // turn OFF OBC
    
    } 
    else // if no error, command voltage and current is set.
    {
      ch_curr_cmd = temp_charge_curr; // set cmd current for OBC
      ch_volt_cmd = charge_volt; // set cmd voltage for OBC
      charge_cmd = OBC_ON; // Turn ON OBC
    }
  
    
}



