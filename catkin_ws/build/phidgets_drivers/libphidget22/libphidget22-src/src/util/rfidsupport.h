/*
* This file is part of libphidget22
*
* Copyright 2020 Phidgets Inc <patrick@phidgets.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, see
* <http://www.gnu.org/licenses/>
*/

#ifndef __RFIDSUPPORT
#define __RFIDSUPPORT

#define RFIDDevice_MAX_TAG_STRING_LEN 25

PhidgetReturnCode RFIDSupport_waitForTag(mosiop_t iop, PhidgetChannelHandle ch, const char *tagString, int timeout, mos_mutex_t* tagLock);
PhidgetReturnCode RFIDSupport_setLatestTagString(mosiop_t iop, PhidgetChannelHandle ch, const char *tagString);

typedef struct {
	
	char latestTagString[RFIDDevice_MAX_TAG_STRING_LEN];

} PhidgetRFIDSupport, *PhidgetRFIDSupportHandle;

void PhidgetRFIDSupport_free(PhidgetRFIDSupportHandle *arg);
PhidgetReturnCode PhidgetRFIDSupport_create(PhidgetRFIDSupportHandle *ir);
void PhidgetRFIDSupport_init(PhidgetRFIDSupportHandle ir);

#endif
