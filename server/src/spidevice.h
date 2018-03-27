/*
* Abstract base class for SPI-attached devices.
*
* Copyright (c) 2013 Micah Elizabeth Scott
* Copyright (c) 2017 Lance Gilbert <lance@lancegilbert.us>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "rapidjson/document.h"
#include "opc.h"
#include "opcdevice.h"
#include <string>
#include <libusb.h> // Also brings in gettimeofday() in a portable way

class SPIDevice : public OPCDevice
{
public:

    SPIDevice(const char *type, bool verbose);
    virtual ~SPIDevice();

    // Must be opened before any other methods are called.
    virtual int open(uint32_t port);

    virtual void write(void* buffer, int length);

    // Overrides from OPCDevice:
    virtual bool matchConfiguration(const Value &config);
    // Note: this must be re-declared here to avoid hiding it.
    virtual void writeMessage(const OPC::Message &msg) = 0;
    virtual void writeMessage(Document &msg);
    virtual void writeColorCorrection(const Value &color);
    virtual void describe(Value &object, Allocator &alloc);

    const char *getTypeString() { return mTypeString; }

protected:
    struct timeval mTimestamp;
    const char *mTypeString;
    bool mVerbose;
    uint32_t mPort;

    // Utilities
    const Value *findConfigMap(const Value &config);
};
