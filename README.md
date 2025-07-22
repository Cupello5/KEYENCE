# KEYENCE Profile Acquisition

This project provides modern C++ example code for acquiring profile data from a **Keyence LJ-X800A controller** and **LJ-X8300 laser sensor** **with a SOFTWARE TRIGGER**.

## Requirements

- **Keyence LJ-X800A controller** with **LJ-X8000 series laser head**
- **Windows** (tested with Visual Studio)
- [Keyence LJ-X8000A Communication Library (DLL & headers)](https://www.keyence.com/support/user/measure/lj-x8000a/software/)
- **LJ-X Navigator** (Keyence configuration software):
- 
  **Configure via LJ-X Navigator:**
  - Go to `Configuration > Direct Setting > Trigger Setting > Trigger mode` and set it to **External Trigger mode**
  - Go to `Configuration > Direct Setting > Profile > Filter process Median` and set it to **OFF**

- Compatible hardware and proper connection/cabling

---

## How to Use

1. **Install the Keyence Communication Library and LJ-X Navigator**  
   - Download the [Keyence Communication Library](https://www.keyence.com/support/user/measure/lj-x8000a/software/)
   - Install **LJ-X Navigator** to configure your controller

2. **Configure the Controller with LJ-X Navigator**  
   - Set `Trigger Mode` to **External Trigger mode**
   - Set `Filter process Median` to **OFF**

3. **Clone this repository and open the code in Visual Studio**

4. **Link your project against the Keyence SDK (.lib, .dll, .h files)**

5. **Compile and Run**

6. **Consult the official Communication Library manual (for API details):**
   [Manual here](https://www.keyence.com/support/user/measure/lj-x8000a/manual/)

---

## Features

- Example in C++
- Connection parameters (IP/port) hardcoded for immediate use
- Reads and parses profile data using a software trigger
- Stores valid data in a vector of `ProfilePoint` (invalid points are ignored)
- Automatic cleanup of resources

### Example: ProfilePoint Structure

```cpp
typedef struct ProfilePoint
{
    double x;            // Position along the scan line (mm)
    double z;            // Height at that x (mm)
    unsigned char intensity; // Light intensity
} ProfilePoint;
