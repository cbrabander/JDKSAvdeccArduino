#include "JDKSAvdeccWorld.h"
#include "Ethernet.h"
#include "SPI.h"
#include "JDKSAvdeccWizNetIO.h"
#include "JDKSAvdeccFrame.h"
#include "JDKSAvdeccADPManager.h"
#include "JDKSAvdeccControlSender.h"

using namespace JDKSAvdecc;

/// This MAC address is within the IEEE Std 1722-2011 assigned range, OUI 90-E0-F0 and is only for example usages.
jdksavdecc_eui48 my_mac = { { 0x90, 0xe0, 0xf0, 0x00, 0x00, 0x01 } };

/// This AVDECC Entity is based on the example MAC address, convered to an EUI-64 by inserting 0xff, 0xfe in the middle
jdksavdecc_eui64 my_entity_id = { {0x90, 0xe0, 0xf0, 0xff, 0xfe, 0x00, 0x01 } };

/// This AVDECC Entity Model ID is for example usages
jdksavdecc_eui64 my_entity_model_id = { {0x90, 0xe0, 0xf0, 0xff, 0xfe, 0x00, 0x02 } };

/// the W5100 chip Raw Ethernet handler object
WizNetIO rawnet(my_mac);

/// The ADP manager is told about the entity id, model_id, entity capabilities, controller capabilities, and valid time in seconds
ADPManager adp_manager( 
  rawnet, 
  my_entity_id, 
  my_entity_model_id, 
  JDKSAVDECC_ADP_ENTITY_CAPABILITY_AEM_SUPPORTED,
  JDKSAVDECC_ADP_CONTROLLER_CAPABILITY_IMPLEMENTED,
  20
  );

/// All of the controls are targetting an example AVDECC Entity ID: 90-e0-f0-ff-fe-00-00-02
jdksavdecc_eui64 target_entity_id = { { 0x90, 0xe0, 0xf0, 0xff, 0xfe, 0x00, 0x00, 0x02 } };

/// All of the controls are targetting an example MAC address: 90-e0-f0-00-00-02
jdksavdecc_eui48 target_mac_address = { { 0x90, 0xe0, 0xf0, 0x00, 0x00, 0x02 } };

/// The shared sequence ID for transmitted messages
uint16_t sequence_id = 0;

/// The mapping of Knob 1 to control descriptor 0x0000. 2 byte payload, refresh time of 1000 ms
ControlSender knob1(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0000,
  2,
  1000
  );

/// The mapping of Knob 2 to control descriptor 0x0001. 2 byte payload, refresh time of 1000 ms
ControlSender knob2(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0001,
  2,
  1000
  );

/// The mapping of Knob 2 to control descriptor 0x0002. 2 byte payload, refresh time of 1000 ms
ControlSender knob3(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0002,
  2,
  1000
  );

/// The mapping of Button 1 to control descriptor 0x0003. 2 byte payload, refresh time of 1000 ms
ControlSender button1(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0003,
  1,
  1000
  );

/// The mapping of Button 2 to control descriptor 0x0004. 2 byte payload, refresh time of 1000 ms
ControlSender button2(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0004,
  1,
  1000
  );

/// The mapping of Button 3 to control descriptor 0x0005. 2 byte payload, refresh time of 1000 ms
ControlSender button3(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0005,
  1,
  1000
  );

/// The mapping of Button 3 to control descriptor 0x0006. 2 byte payload, refresh time of 1000 ms
ControlSender button4(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0006,
  1,
  1000
  );

/// The mapping of Button 3 to control descriptor 0x0007. 2 byte payload, refresh time of 1000 ms
ControlSender button5(
  rawnet,
  my_entity_id,
  target_entity_id,
  target_mac_address,
  sequence_id,
  0x0007,
  1,
  1000
  );


/// The ValueMapper scans the A/D's and Digital input pins at the specified rate
/// and applies the new values to the appropriate knob or button
class ValueMapper : public Handler {
private:
  /// The update rate in milliseconds
  uint32_t m_update_rate_in_millis;
  
  /// The time that the last update happened
  uint32_t m_last_update_time;
public:  

  ValueMapper( uint32_t update_rate_in_millis ) 
  : m_update_rate_in_millis(update_rate_in_millis) 
  , m_last_update_time(0) {
  }
  
  virtual void Tick( uint32_t time_in_millis ) {
    if( time_in_millis > m_last_update_time + m_update_rate_in_millis ) {
      m_last_update_time = time_in_millis;
      
      knob1.SetValueDoublet( analogRead(0) << 6 ); // Scale 10 bit analog reading into 0-0xffc0
      knob2.SetValueDoublet( analogRead(1) << 6 );
      knob3.SetValueDoublet( analogRead(2) << 6 );
      
      button1.SetValueOctet( digitalRead(2) ? 0x00 : 0xff ); // DLI reads true when not pressed, reverse logic
      button2.SetValueOctet( digitalRead(3) ? 0x00 : 0xff );
      button3.SetValueOctet( digitalRead(4) ? 0x00 : 0xff );
      button4.SetValueOctet( digitalRead(5) ? 0x00 : 0xff );  
      button5.SetValueOctet( digitalRead(6) ? 0x00 : 0xff );  
  
      digitalWrite( A5, !digitalRead(2) );
      digitalWrite( A4, !digitalRead(3) );      
    }
  }
};

/// Create the mapper which polls at 50 ms
ValueMapper value_mapper(50);

/// Create a HandlerGroup which can manage up to 16 handlers
HandlerGroup<16> all_handlers(rawnet);

void setup() {
  // Set up the I/O pins for 3 knobs, 5 buttons, and 2 LED's
  pinMode(9,OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);
  
  // Initialize the W5100 chip 
  rawnet.Initialize();
  
  // Put all the handlers into the HandlerGroup
  
  all_handlers.Add( &value_mapper );
  all_handlers.Add( &adp_manager );
  all_handlers.Add( &knob1 );
  all_handlers.Add( &knob2 );
  all_handlers.Add( &knob3 );
  all_handlers.Add( &button1 );
  all_handlers.Add( &button2 );
  all_handlers.Add( &button3 );  
  all_handlers.Add( &button4 );
  all_handlers.Add( &button5 );
}


void loop() {
  // Get the current time in milliseconds
  uint32_t cur_time = millis();
  
  // Tell all the handlers to do their periodic jobs
  all_handlers.Tick(cur_time);
}

