

void UART_task_setup()
{
  
  Serial.println("This is UART Task Set Up.");
  //SERIAL.begin(115200);
  //delay(100);
  //while (!SERIAL) ; 
}

static void UART_task(void *pvParameters)
{
  while(1) 
  {
  int measurement;
  Serial.println("*******************************************************UART Task Start ************************************************.");
  digitalWrite(LED_BUILTIN, HIGH);
  measurement = uxTaskGetStackHighWaterMark( Handle_UART_task );
		SERIAL.print("UART Stack: ");
		SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark( Handle_CP_task );
		SERIAL.print("CP Stack: ");
		SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark( Handle_PROXI_task );
		SERIAL.print("PROXI Stack: ");
		SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark( Handle_BMS_task );
		SERIAL.print("BMS Stack: ");
		SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark( Handle_OBC_task );
		SERIAL.print("OBC Stack: ");
		SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark( Handle_HMI_task );
		SERIAL.print("HMI Stack: ");
		SERIAL.println(measurement);
    /*
    measurement = uxTaskGetStackHighWaterMark( Handle_BLE_task );
		SERIAL.print("BLE Stack: ");
		SERIAL.println(measurement); */

      SERIAL.println("");			 
    	SERIAL.println("****************************************************");
    	SERIAL.print("Free Heap: ");
    	SERIAL.print(xPortGetFreeHeapSize());
    	SERIAL.println(" bytes");

    	SERIAL.print("Min Heap: ");
    	SERIAL.print(xPortGetMinimumEverFreeHeapSize());
    	SERIAL.println(" bytes");
    	SERIAL.flush();
      digitalWrite(LED_BUILTIN, LOW);

  Serial.println("*******************************************************UART Task END ************************************************.");
  //taskYIELD();
    myDelayMs(10000);
  }
}