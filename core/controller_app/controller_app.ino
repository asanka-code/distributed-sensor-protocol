// For running on Arduino, commented the below line.
//#include <SPI85.h>
#include <SPI.h>

#include <Mirf.h>
#include <nRF24L01.h>

// For running on Arduino, commented the below line.
//#include <MirfHardwareSpi85Driver.h>
#include <MirfHardwareSpiDriver.h>

struct packet_struct {	
	byte system_id;		// 1 byte
	byte component_id;	// 1 byte
	int data;		// 2 bytes
};

void setup () {

		// Only for Arduino		
		Serial.begin(9600);

                // For running on Arduino, commented the below line.
                //Mirf.cePin = PB4;
                //Mirf.csnPin = PB3;

                // For running on Arduino, commented the below line.
                //Mirf.spi = &MirfHardwareSpi85;
                Mirf.spi = &MirfHardwareSpi;

                Mirf.init();
  
                Mirf.setRADDR((byte *)"disp");   
                Mirf.payload = sizeof(packet_struct);              
                
		Mirf.channel = 50;
   
                Mirf.config();        
}

void loop () {
        
        byte carrier_detect_reg_value = (byte) 0;        
        static packet_struct packet;                                   
        
         //RX mode
         Mirf.ceHi();
         Mirf.readRegister(CD, &carrier_detect_reg_value, sizeof(carrier_detect_reg_value));
         Serial.print("carrier_detect_reg_value = ");
         Serial.println(carrier_detect_reg_value, BIN);
                
          // process the received sensor data
          Mirf.getData((byte *) &packet);
          Serial.print("component ID = ");
          Serial.println(packet.component_id);
          Serial.print("data = ");
          Serial.println(packet.data);
                
           // wait for a while before sampling the sensors in the next iteration                
           delay(500);
} 
  
  
  
