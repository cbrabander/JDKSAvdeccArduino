/*
  Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   3. Neither the name of J.D. Koftinoff Software, Ltd. nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "JDKSAvdeccWorld.hpp"
#include "JDKSAvdeccControlSender.hpp"

namespace JDKSAvdecc {

ControlSender::ControlSender(
                 jdksavdecc_eui64 const &entity_id,
                 jdksavdecc_eui64 const &target_entity_id,
                 jdksavdecc_eui48 const &target_mac_address,
                 uint16_t &sequence_id,
                 uint16_t target_descriptor_index,
                 uint16_t value_length,
                 jdksavdecc_timestamp_in_milliseconds update_rate_in_millis
                 )
: m_entity_id( entity_id )
, m_target_entity_id( target_entity_id )
, m_target_mac_address( target_mac_address )
, m_sequence_id( sequence_id )
, m_target_descriptor_index( target_descriptor_index )
, m_value_length(value_length)
, m_update_rate_in_millis(update_rate_in_millis)
, m_last_send_time_in_millis(0) {
    m_value[0] = 0;
    m_value[1] = 0;
    m_value[2] = 0;
    m_value[3] = 0;
}

void ControlSender::SetValueOctet( uint8_t val ) {
    if( val!=m_value[0] ) {
        m_last_send_time_in_millis -= m_update_rate_in_millis;
        m_value[0] = val;
    }
}

/// Set a doublet value. If it actually changed, then force Tick to send ASAP
void ControlSender::SetValueDoublet( uint16_t val ) {
    if( (((val>>8)&0xff) !=m_value[0])
       || (((val>>0)&0xff)!=m_value[1]) ) {
        m_last_send_time_in_millis -= m_update_rate_in_millis;
        m_value[0] = (val>>8)&0xff;
        m_value[1] = (val>>0)&0xff;
    }
}

/// Set a quadlet value. If it actually changed, then force Tick to send ASAP
void ControlSender::SetValueQuadlet( uint32_t val ) {
    if( (((val>>24)&0xff) !=m_value[0])
       || (((val>>16)&0xff)!=m_value[1])
       || (((val>>8)&0xff) !=m_value[2])
       || (((val>>0)&0xff)!=m_value[3]) ) {

        m_last_send_time_in_millis -= m_update_rate_in_millis;
        m_value[0] = (val>>24)&0xff;
        m_value[1] = (val>>16)&0xff;
        m_value[2] = (val>>8)&0xff;
        m_value[3] = (val>>0)&0xff;
    }
}

void ControlSender::Tick( jdksavdecc_timestamp_in_milliseconds time_in_millis ) {
    if( WasTimeOutHit(time_in_millis,m_last_send_time_in_millis,m_update_rate_in_millis) ) {
        SendSetControl();
        m_last_send_time_in_millis = time_in_millis;
    }
}

void ControlSender::SendSetControl() {
    Frame<128> pdu(0,m_target_mac_address,net->GetMACAddress(),JDKSAVDECC_AVTP_ETHERTYPE);  // DA, SA, EtherType, ADPDU

    // AECPDU common control header
    pdu.PutOctet( 0x80 + JDKSAVDECC_SUBTYPE_AECP); // cd=1, subtype=0x7b (AECP)
    pdu.PutOctet( 0x00 + JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND );  // sv=0, version=0, message_type = AEM_COMMAND
    pdu.PutOctet( ((JDKSAVDECC_AEM_STATUS_SUCCESS) << 3) + 0 ); // Send success code. top 3 bits of control_data_length is 0
    pdu.PutOctet( JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_LEN - JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + m_value_length ); // control_data_length field is N + value_length

    pdu.PutEUI64( m_target_entity_id ); // entity_id of the device we are setting
    pdu.PutEUI64( m_entity_id ); // controller_id

    pdu.PutDoublet( m_sequence_id );
    pdu.PutDoublet( JDKSAVDECC_AEM_COMMAND_SET_CONTROL );
    pdu.PutDoublet( JDKSAVDECC_DESCRIPTOR_CONTROL );
    pdu.PutDoublet( m_target_descriptor_index );
    pdu.PutBuf( m_value, m_value_length );

    net->SendRawNet( pdu.GetBuf(), pdu.GetPos() );
    m_sequence_id++;
}

bool ControlSender::ReceivedPDU( jdksavdecc_timestamp_in_milliseconds time_in_millis, uint8_t *buf, uint16_t len ) {
    return false;
}


}
