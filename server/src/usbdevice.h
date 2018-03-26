/*
 * Abstract base class for USB-attached devices.
 *
 * Copyright (c) 2013 Micah Elizabeth Scott
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


/*
 * We find it important to know whether the libusbx backend ends up copying or mapping
 * our transaction data when it's submitted. On Linux, the kernel must already do a copy
 * to get our userspace data into kernel space. On Windows and Mac OS, the user buffer
 * is mapped. This matters because any changes to the buffer while a transfer is queued
 * will cause the transfer to change. This causes tearing for us.
 *
 * We can avoid this by copying the buffer ourselves, but we'd prefer to avoid the CPU
 * overhead of copying the buffer twice, so we only do this on platforms where the kernel
 * isn't already copying it.
 */

#ifdef OS_LINUX
 // No need to copy the buffer
#elif OS_WINDOWS
  #define NEED_COPY_USB_TRANSFER_BUFFER 1
#elif OS_DARWIN
  #define NEED_COPY_USB_TRANSFER_BUFFER 1
#else
  #error Dont know whether we need to copy the USB transfer buffer
#endif


class USBDevice : public OPCDevice
{
public:
    USBDevice(libusb_device *device, const char *type, bool verbose);
    virtual ~USBDevice();

    // Overrides from OPCDevice:
    virtual bool probeAfterOpening() override;
    virtual bool matchConfiguration(const Value &config) override;
    virtual void writeMessage(const OPC::Message &msg) override = 0;
    virtual void writeMessage(Document &msg) override;
    virtual void writeColorCorrection(const Value &color) override;
    virtual void describe(Value &object, Allocator &alloc) override;

    libusb_device *getDevice() { return mDevice; };
    const char *getSerial() override { return mSerialString; }
    const char *getTypeString() override { return mTypeString; }

protected:
    libusb_device *mDevice;
    libusb_device_handle *mHandle;
    struct timeval mTimestamp;
    const char *mTypeString;
    const char *mSerialString;
    bool mVerbose;

    // Utilities
    const Value *findConfigMap(const Value &config);
};
