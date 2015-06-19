/*
 *  Multi2Sim
 *  Copyright (C) 2014  Amir Kavyan Ziabari (aziabari@ece.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef NETWORK_FRAME_H
#define NETWORK_FRAME_H

#include <lib/esim/Event.h>

namespace net
{

class Frame : public esim::Frame
{

	// Packet
	Packet *packet;

	// The event to be scheduled when the message arrived
	esim::Event *return_event;

public:

	/// Constructor
	Frame(Packet *packet, esim::Event *return_event) :
		packet(packet),
		return_event(return_event)
	{
	}

	/// Return the packet
	Packet *getPacket() const { return packet; }

	/// Get the return event type
	esim::Event *getReturnEvent() { return return_event; }
};

}

#endif
