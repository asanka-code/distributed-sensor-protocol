// For running on Arduino, commented the below line.
#include <SPI85.h>
//#include <SPI.h>

#include <Mirf.h>
#include <nRF24L01.h>

// For running on Arduino, commented the below line.
#include <MirfHardwareSpi85Driver.h>
//#include <MirfHardwareSpiDriver.h>

struct packet_struct {	
	byte system_id;		// 1 byte
	byte component_id;	// 1 byte
	int data;		// 2 bytes
};

void setup () {

		// Only for Arduino		
		//Serial.begin(9600);

                // For running on Arduino, commented the below line.
                Mirf.cePin = PB4;
                Mirf.csnPin = PB3;

                // For running on Arduino, commented the below line.
                Mirf.spi = &MirfHardwareSpi85;
                //Mirf.spi = &MirfHardwareSpi;

                Mirf.init();
  
                Mirf.setRADDR((byte *)"disp");   
                Mirf.payload = sizeof(packet_struct);              
		
                Mirf.channel = 50;
   
                Mirf.config();        
}

void loop () {
        
        static packet_struct packet;
        int my_system_id = 0;        
        int my_component_id = 0;
        //int my_component_id = 1;
        
         // wait for a request packet
         while (!Mirf.dataReady()) { }
         
         // check whether it's for me
         Mirf.getData((byte *) &packet);
         if ( (packet.system_id!=my_system_id) || (packet.component_id!=my_component_id) ) {
                 return;
         }
         
         // Only for Arduino
         //Serial.print("Sensor: ");
         //Serial.print(my_component_id);
         //Serial.println(" Received a packet from controller mote.");
         
         // read sensor data and fill the packet
         //int sensor_data = analogRead(0);
         int sensor_data = 38;
         packet.data = sensor_data;
         
          // sending data packet
          Mirf.setTADDR((byte *)"disp"); 
          Mirf.send((byte *) &packet);

          // wait till the sending is done  
          while (Mirf.isSending()) { }
          //Serial.print("Sensor: ");
          //Serial.print(my_component_id);
          //Serial.println(" Sent a packet to the controller mote.");
} 
  
  
  
