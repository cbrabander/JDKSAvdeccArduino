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
#include "JDKSAvdeccHandler.hpp"

namespace JDKSAvdecc {


/// The HandlerGroupBase class maintains a list of Handler pointers
/// Dispatches Tick() and ReceivedPDU() calls to all Handlers
/// It does not contain the storage of the handlers
/// No bounds checking is done
class HandlerGroupBase : public Handler {
protected:
    uint16_t m_num_items;
    Handler **m_item;
    uint32_t m_rx_count;
    uint32_t m_handled_count;
public:
    HandlerGroupBase(Handler **item_storage);

    /// Add a handler to the list
    void Add( Handler *v ) {
        m_item[m_num_items++] = v;
    }

    uint32_t GetRxCount() const { return m_rx_count; }
    uint32_t GetHandledCount() const { return m_handled_count; }

    // Poll the NetIO object for an incoming frame. If it is multicast, or m
    virtual bool PollNet( jdksavdecc_timestamp_in_milliseconds time_in_millis );

    /// Send Tick() messages to all encapsulated Handlers
    /// and poll incoming network for PDU's and dispatch them
    virtual void Tick( jdksavdecc_timestamp_in_milliseconds time_in_millis );

    /// Send ReceivedPDU message to each handler until one returns true.
    virtual bool ReceivedPDU( jdksavdecc_timestamp_in_milliseconds time_in_millis, uint8_t *buf, uint16_t len );
};


/// HandlerGroup is a HandlerGroupBase and contains
/// the storage of the contained Handler pointers.
/// The HandlerGroup is templatelized by the MaxItem count.
template <uint16_t MaxItems>
class HandlerGroup : public HandlerGroupBase {
private:
    Handler *m_item_storage[MaxItems];
public:
    HandlerGroup()
        : HandlerGroupBase(m_item_storage) {}
};

}

