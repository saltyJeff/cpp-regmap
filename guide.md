# Usage Guide

## 1. Declare your bus:
See [bus.h](include/regmap/bus.h) for the interface you need to
implement. This is intrinsically linked to your hardware (maybe
your device uses UART, I2C, or SPI). For an I2C device, the
lower 10 bits of `deviceAddr` could be the I2C address, and for
an SPI device, it could be the chip-select pin.

Make sure that your functions return a negative number on error.
This convention is adopted by the rest of the library.

## 2. Declare your registers:
In a header file, include the [regmap.h](include/regmap/regmap.h) file.

Using the `DECLR_REG(NAME, ADDR, SZ)` macro, define your register like so:
```c++
DECLR_REG(WHOAMI, 0x00, uint8_t)
```
This defines a register named `WHOAMI` located at address 0, with size `uint8_t`.
A convenience macro is provided: `DECLR_BYTE(NAME, ADDR)` which is just
`DECLR_REG(NAME, ADDR, uint8_t)`.

## 3. Declare your masks:
Some variables are not given an entire byte, but instead only get a subfield of bits.
Let's say that `WHOAMI` is actually 2 registers: `MANUFACTURER = WHOAMI[7:4]` and
`REVISION = WHOAMI[3:0]`. Using the `DECLR_MASK(NAME, REG, HIGH, LOW)` macro, define
these masks like so:
```c++
DECLR_MASK(MANUFACTURER, WHOAMI, 7, 4)
DECLR_MASK(REVISION, WHOAMI, 3, 0)
```

## 4. Define your register map
Your register map will define the device you wish to abstract. The class `Regmap<E, M...>`
takes in 2 type parameters:
* `E`: The endianness of the device. Set to either `endian::big` or `endian::little`.
* `M...`: A list of registers you would like memoized. 

Memoization stores the value of a register each time you read from it or write to it. 
This can help cut down on unneeded bus transactions. 
I suggest that you memoize configuration registers or factory-programmed ones, and avoid
memoizing registers that change with the environment (like accelerometer ones).

`Regmap` also takes in 2 constructor arguments, a pointer to a `Bus` which will actually
perform the transactions, and a `deviceId` to identify the different instances of the same `Regmap`.

## 5. Interact with the register map
I'll define `Regmap` as follows:
```c++
// In practice, you should supply a bus instead of nullptr
Regmap<endian::big, WHOAMI, ANOTHER_REGISTER, BLAH_BLAH> regmap(nullptr, 0);
```
Interaction is compile-time optimized to provide a small stack footprint,
but somewhat larger binaries. You can read and write entire registers
using:
```c++
uint8_t whoamiResult;
regmap.read<WHOAMI>(whoamiResult); // returns 0 on success, < 0 on bus error
```
To read a mask instead, just substitute the mask! :
```c++
uint8_t manufacturerResult;
regmap.read<MANUFACTURER>(manufacturerResult); // guaranteed ok (see below for why)
```
Because we've memoized the `WHOAMI` register, the call to read `MANUFACTURER` does not touch
the bus at all! (see my unit test if you don't believe me). We can guarantee the 2nd
read happens if the first one went through. Thanks C++17!

Writing is the same, except you change the name of the method from `read` to `write`:
```c++
regmap.write<MANUFACTURER>(15); // returns 0 on success, < 0 on bus error
```
## 6. Merging masks together
What if we want to write `MANUFACTURER` and `REVISION` at the same time. Should we do it in 1 transaction,
but require the developer to know that they lie on the same register? Or should we write 1 at a time and burn 2
transactions? C++ 17 says no! we can do both at the same time. You will need to use the `MaskVal` type.
```
using manufacturerValue = MaskVal<MANUFACTURER, 2>;
using revisionValue = MaskVal<REVISION, 1>;
using merged = MergeMaskVals<manufacturerValue, revisionValue>;
regmap.write<merged>(); // will set WHOAMI to 0x21
```
The `Regmap` is smart enough to see that you're completely rewriting `WHOAMI`, and will do this
in a single write transaction. In fact, this observation is made at compile time. Thanks C++17!

Another compile-time bonus: if you try to merge masks from 2 different registers, it will be
a compile-time fault.