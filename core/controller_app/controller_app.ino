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
	byte component_id;	        // 1 byte
	int data;		                // 2 bytes

	byte tx_power_level;	// 1 byte
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
                    
        static packet_struct packet;
        
        // set the number of sensor nodes that belongs to this mother mote
        int num_sensors = 2;
        //int num_sensors = 1;        
        
        int num_power_levels = 4;        

        for (int sensor_number = 0; sensor_number<num_sensors; sensor_number++) {
                              
                for (int tx_power = 0; tx_power<num_power_levels; tx_power++) {                                
                                               
                        // generating request packet
                        packet.system_id = 0;
                        packet.component_id = sensor_number;
                        packet.data = 0;
                        packet.tx_power_level = tx_power;
                
                        // sending request packet
                        Mirf.setTADDR((byte *)"disp"); 
                        Mirf.send((byte *) &packet);
                
                        // wait till the sending is done  
                        while (Mirf.isSending()) { }
                        Serial.print("Controller: Sent a request packet to the sensor:");
                        Serial.println(sensor_number);

                        // take note of the time we send this packet
                        unsigned long time = millis();
                            
                        // wait for the response data packet
                        while (!Mirf.dataReady()) {
                        
                                // it no response for before timeout
                                if ((millis() - time) > 500) {
                                        break;
                                }
                        }
                
                       if (!Mirf.dataReady()) {
                       
                               // if we came here because of no response from sensor, we ask it to increase transmission power
                       
                               Serial.println("----------------------------------------------------------------");
                               Serial.print("Not received from sensor:");
                               Serial.println(sensor_number);                
                               Serial.print("component ID = ");
                               Serial.println(packet.component_id);
                               Serial.print("data = ");
                               Serial.println(packet.data);
                               Serial.print("tx_power_level = ");
                               Serial.println(packet.tx_power_level);
                               Serial.println("----------------------------------------------------------------");                       
                       
                       } else {
                       
                                Mirf.getData((byte *) &packet);

                                // if we have received a response from the sensor, we print its details
                                Serial.println("----------------------------------------------------------------");
                                Serial.print("Received from sensor:");
                                Serial.println(sensor_number);                
                                Serial.print("component ID = ");
                                Serial.println(packet.component_id);
                                Serial.print("data = ");
                                Serial.println(packet.data);
                                Serial.print("tx_power_level = ");
                                Serial.println(packet.tx_power_level);
                                Serial.println("----------------------------------------------------------------");
                       }
                
                        // wait for a while before sampling the sensor with next power level
                        //delay(1000);
                        delay(500);
                }
                
                // wait for a while before sampling the next sensor
                //delay(1000);
                delay(500);
        }
} 
  
  
  
