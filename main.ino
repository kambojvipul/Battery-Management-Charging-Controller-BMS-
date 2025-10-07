#define MAX_TEMP 60
#define MIN_TEMP -10
#define OBC_ON 0
#define OBC_OFF 1
//#define SERIAL1_RX_BUFFER_SIZE 6
#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Adafruit_SleepyDog.h>
#include <FreeRTOS_SAMD21.h>
#include <CAN.h>
#include <CAN2.h>

/////////////////////Variables////////////////


#define SERIAL         Serial //SerialUSB

/////////bat bariables//////////////
uint8_t inv_switch, dc12v_switch;
int16_t bat_volt, bat_current;
uint16_t bat_energy;
uint16_t bat_capacity_ah, bat_dis_cycle, bat_soc;
unsigned short bat_connection;
int16_t NTC1, NTC2, NTC3, NTC4, NTC5;
int16_t bat_minTemp, bat_maxTemp, box_temp;
uint16_t cell_bal;
uint16_t cell_volt[16];
uint16_t BMS_rxerr = 0;

////////////OBC//////////////////
unsigned short obc_com_rxerr = 0;
uint16_t ch_volt_cmd, ch_curr_cmd = 0;
uint16_t ch_volt_fb, ch_curr_fb = 0;
uint8_t charge_cmd = OBC_OFF;
uint8_t OBC_dto_code = 0;
int16_t charge_temp = 0;
uint8_t over_volt_err = 0;
uint8_t over_curr_err = 0;
uint8_t over_temp_err = 0;
uint8_t under_temp_err = 0;
uint16_t ch_comb_err = 0;
uint16_t charge_volt = 51;
/////////////////////////////

///////////CP///////////
unsigned short max_charge_curr = 0;
unsigned int duty_avg;
unsigned short charge_duty_error = 0;
uint16_t freq_avg = 0;
uint16_t charge_time_left = 0;
///////////////////////////
 void UART_task_setup(void);
 void CAN_COMM_SETUP(void);
 void CP_task_setup(void);
 void PROXI_task_setup(void);
 void BMS_task_setup(void);
 void OBC_task_setup(void);
 void HMI_task_setup(void);
 void IO_task_setup(void);

///////////////////////////


/////////////////////Task Handle//////////////////
TaskHandle_t Handle_UART_task;
TaskHandle_t Handle_CP_task;
TaskHandle_t Handle_PROXI_task;
TaskHandle_t Handle_BMS_task;
TaskHandle_t Handle_OBC_task;
TaskHandle_t Handle_HMI_task;
TaskHandle_t Handle_IO_task;
//TaskHandle_t Handle_WATCHDOG_task;
/////////////////////////////////////////////////////

void myDelayUs(int us)
{
  vTaskDelay( us / portTICK_PERIOD_US );  
}

void myDelayMs(int ms)
{
  vTaskDelay( (ms * 1000) / portTICK_PERIOD_US );  
}

void myDelayMsUntil(TickType_t *previousWakeTime, int ms)
{
  vTaskDelayUntil( previousWakeTime, (ms * 1000) / portTICK_PERIOD_US );  
}

//static void WATCHDOG_task(void *pvParameters) { while(1) Watchdog.reset(); myDelayMs(7900);}

void setup() {
  ///////////////////Setting Up Tasks///////////////
  Watchdog.enable(8000); // Watchdog timer start 8 second reset

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected
  digitalWrite(LED_BUILTIN, HIGH); //Turn On Led before setting up of Tasks

  Serial.begin(115200);
  UART_task_setup();
  CAN_COMM_SETUP();
  CP_task_setup();
  PROXI_task_setup();
  BMS_task_setup();
  OBC_task_setup();
  HMI_task_setup();
  IO_task_setup();

  //BLE_task_setup();

  ///////////////////////////////Create Tasks to run/////////////////////////

  //xTaskCreate(  UART_task,      "UART Task",     256, NULL,   tskIDLE_PRIORITY + 1,   &Handle_UART_task );
  xTaskCreate(  CP_task,        "CP Task",       2*128, NULL,   tskIDLE_PRIORITY + 2,   &Handle_CP_task   );
  xTaskCreate(  PROXI_task,     "PROXI Task",    2*128, NULL,   tskIDLE_PRIORITY + 1,   &Handle_PROXI_task);
  //xTaskCreate(  BMS_task,       "BMS Task",      2*512, NULL,   tskIDLE_PRIORITY + 1,   &Handle_BMS_task  );
  xTaskCreate(  OBC_task,       "OBC Task",      2*256, NULL,   tskIDLE_PRIORITY + 1,   &Handle_OBC_task  );
  xTaskCreate(  HMI_task,       "HMI Task",      128, NULL,   tskIDLE_PRIORITY + 1,   &Handle_HMI_task  );
  xTaskCreate(  IO_task,       "IO Task",      128, NULL,   tskIDLE_PRIORITY + 1,   &Handle_IO_task  );



digitalWrite(LED_BUILTIN, LOW);  //////////////Turn Off Led after Task Create, if something happen, led remain ON///////////////////////

  ///////////////////////Start Scheduler to run all Tasks////////////////////////
  vTaskStartScheduler();
    while(1)
  {
	  SERIAL.println("Scheduler Failed! \n");
	  SERIAL.flush();
	  delay(1000);
  }
  /////////////////////////////////////////////////////////////////
}

void loop() {
    Serial.print("."); //print out dots in terminal, we only do this when the RTOS is in the idle state
    SERIAL.flush();
    Watchdog.reset(); // In Idle Task, Watchdog timer reset
    delay(500); //delay is interrupt friendly, unlike vNopDelayMS
}


