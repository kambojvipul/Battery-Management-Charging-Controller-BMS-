// Setting up TWO Can Channels in the Board using SPI
// CAN 500 and 250 Kbps

void CAN_COMM_SETUP()
{

    while(1)
    {
      // start the CAN bus at 500 kbps for BMS
      if (!CAN.begin(500E3)) 
      {
        Serial.println("Starting 500 Kbps CAN failed!");
        delay(1000);
        continue;
      }

    // start the CAN bus at 250 kbps for OBC
      if (!CAN2.begin(250E3))  //250kbps raw board 
      {
        Serial.println("Starting 250 Kbps CAN failed!");
        delay(1000);
        continue;
      }
      Serial.println("Both CAN Successfully Started.");
      break;
    }

}