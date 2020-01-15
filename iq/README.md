# iqmotor-c
Posix C framework for controlling IQ motors

This repository contains a set of simplified APIs for the IQ Control motor libraries.

I found the native .cpp docs for the IQ Control motor libraries to be a bit confusing,
so I wrote iqmotor.c to create a set of simplified APIs for motion control.

The libraries also implement all the "low level serial interface" stuff that is
omitted from the [IQ Control documentation](https://storage.googleapis.com/wzukusers/user-18643913/documents/5c957fc73ed03u3q7OTy/IQ2306_communication_manual_position_firmware.pdf). Of course their docs try to be as generic as possible, so this
implementation makes the following assumptions about your environment:

* You're running under Linux
* You're using a USB-to-serial adapter (like the FTDI TTL-232R-RPi),
and that it appears at a device node like /dev/ttyUSB0
* You're using the command line

The file simplest_demo.cpp is a good starting point to understand the
API. Each motor you're using will require its own serial cable (the
full CPP library apparently supports chaining multiple motors on a
single serial interface, but I could never figure out how to get that
to work).

Each motor needs a control structure:

```c
  struct iqMotor *motor0;
```

And the control structure should be initialized exactly once:

```c
  motor0 = iqCreateMotor( "/dev/ttyUSB0" );
```

Once the control structure has been allocated, you can pass it as
an argument to a library of functions, for example:

```c
  iqSetAngle(motor0, 3.14 * 10, 2000);
```

This command will turn the motor 5 revolutions (one rev = 2 * pi) over a period
of two seconds.

The API docs are kept up to date in iqmotor.h, you can find information
on other simple C API functions implemented so far there.

