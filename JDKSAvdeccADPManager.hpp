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
#pragma once

#include "JDKSAvdeccWorld.hpp"
#include "JDKSAvdeccNetIO.hpp"
#include "JDKSAvdeccFrame.hpp"

#include "JDKSAvdeccHandler.hpp"

namespace JDKSAvdecc {

class ADPManager : public Handler {
public:

    /// Construct the ADPManager object
    ADPManager(NetIO &net,
               jdksavdecc_eui64 const &entity_id,
               jdksavdecc_eui64 const &entity_model_id,
               uint32_t entity_capabilities,
               uint32_t controller_capabilities,
               uint16_t valid_time_in_seconds );

    /// Send the ENTITY_AVAILABLE message if it is time to
    virtual void Tick( jdksavdecc_timestamp_in_milliseconds time_in_millis );

    /// Handle any incoming ADPDU. Return true if handled
    virtual bool ReceivedPDU( jdksavdecc_timestamp_in_milliseconds time_in_millis, uint8_t *buf, uint16_t len );

    /// Formulate the ADPDU and send it
    void SendADP();

    jdksavdecc_eui64 const &GetEntityID() const { return m_entity_id; }
    jdksavdecc_eui64 const &GetEntityModelID() const { return m_entity_model_id; }
    uint32_t GetEntityCapabilities() const { return m_entity_capabilities; }
    uint32_t GetControllerCapabilities() const { return m_controller_capabilities; }
    uint16_t GetValidTimeInSeconds() const { return m_valid_time_in_seconds; }
    uint32_t GetAvailableIndex() const { return m_available_index; }

protected:
    NetIO &m_net;
    jdksavdecc_eui64 m_entity_id;
    jdksavdecc_eui64 m_entity_model_id;
    uint32_t m_entity_capabilities;
    uint32_t m_controller_capabilities;
    uint16_t m_valid_time_in_seconds;
    uint32_t m_available_index;
    jdksavdecc_timestamp_in_milliseconds m_last_send_time_in_millis;
};

}
