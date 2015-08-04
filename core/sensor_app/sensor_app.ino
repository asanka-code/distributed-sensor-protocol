/*
nRF24L01 to ATtiny85
MISO -> PB0
MOSI -> PB1
SCK -> PB2
CE -> PB4
CSN -> PB3
*/


// For running on Arduino, commented the below line.
#include <SPI85.h>
//#include <SPI.h>

#include <Mirf.h>
#include <nRF24L01.h>

// For running on Arduino, commented the below line.
#include <MirfHardwareSpi85Driver.h>
//#include <MirfHardwareSpiDriver.h>

// the bits which control Tx power levels of RF_SETUP register
#define RF_PWR_LOW        1
#define RF_PWR_HIGH        2

// variable to hold the content of RF_SETUP register
byte rf_setup_reg_value = (byte) 0;

struct packet_struct {	
	byte system_id;		// 1 byte
	byte component_id;	// 1 byte
	int data;		// 2 bytes

	byte tx_power_level;	// 1 byte
};

struct compressed_packet{
    byte addresses;     // 2:4:2 system_id:component_id,tx_power_level
    int data;
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
                Mirf.payload = sizeof(compressed_packet);              
		
                Mirf.channel = 50;                                                           
                   
                Mirf.config();        
}

void loop () {
        
        static packet_struct packet;
        static compressed_packet compressed;

        int my_system_id = 0;        
        int my_component_id = 0;
        //int my_component_id = 1;
        
         // wait for a request packet
         while (!Mirf.dataReady()) { }
         
         // check whether it's for me
         Mirf.getData((byte *) &compressed);
         unpack(&packet, &compressed);

         if ( (packet.system_id!=my_system_id) || (packet.component_id!=my_component_id) ) {
                 return;
         }
         
         // read the content of the RF_SETUP register               
         Mirf.readRegister(RF_SETUP, &rf_setup_reg_value, sizeof(rf_setup_reg_value));
                       
         // configure Tx power according to the instructions from mother mote
         if(packet.tx_power_level == 0) {                 
                 // Set to -18dB Tx power (bits '00')
                 rf_setup_reg_value &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;                 
         } else if(packet.tx_power_level == 1) {
                 // Set to -12dB Tx power (bits '01')
                 rf_setup_reg_value &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;                 
                 rf_setup_reg_value |= _BV(RF_PWR_LOW) ;                 
         } else if(packet.tx_power_level == 2) {                 
                 // Set to -6dB Tx power (bits '10')
                 rf_setup_reg_value &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;                 
                 rf_setup_reg_value |= _BV(RF_PWR_HIGH) ;                 
         } else if(packet.tx_power_level == 3) {         
                 // Set to 0dB Tx power (bits '11')
                 rf_setup_reg_value &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;                 
                 rf_setup_reg_value |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
         }                 
         
        // write the content to the RF_SETUP register               
        Mirf.writeRegister(RF_SETUP, &rf_setup_reg_value, sizeof(rf_setup_reg_value));
                
                
         
         // Only for Arduino
         //Serial.print("Sensor: ");
         //Serial.print(my_component_id);
         //Serial.println(" Received a packet from controller mote.");
         
         // read sensor data and fill the packet
         //int sensor_data = analogRead(0);
         int sensor_data = 38;
         packet.data = sensor_data;
         
         pack(&compressed, &packet);
          // sending data packet
          Mirf.setTADDR((byte *)"disp"); 
          Mirf.send((byte *) &compressed);

          // wait till the sending is done  
          while (Mirf.isSending()) { }
          //Serial.print("Sensor: ");
          //Serial.print(my_component_id);
          //Serial.println(" Sent a packet to the controller mote.");
} 
  
void pack(struct compressed_packet *compressed, struct packet_struct * pac){
    int addr;

    addr = pac->system_id;
    addr <<=4;
    addr |= pac->component_id;    
    addr <<=2;
    addr |= pac->tx_power_level;

    compressed->addresses = addr;
    compressed->data = pac->data;


}

void unpack(struct packet_struct *pac, struct compressed_packet *compressed){
    pac->system_id = byte(compressed->addresses>>6);
    pac->component_id = byte((compressed->addresses & B111100 ) >> 2);
    pac->tx_power_level = byte(compressed->addresses & B11);
    pac->data = compressed->data;
}
  
