# HAL Library for the Padauk PFS154 Microcontroller

### platformio.ini
```ini
[env:development]
platform = https://github.com/TecMic/platformio-padauk
board = pfs154
framework = easypdk, easypdk-hal

build_flags = 
    # -DSYS_TIME_USE_TIM2=1
...
```

### Global Defines
Possible defines to add to the build_flags in platformio.ini

#### SYS_TIMER
By default the sys-timer uses Timer 16 with a interrupt every 1.02 ms.\
Timer 2 can be used for interrupts every 1.00 ms.
>Note: Timer 2 is used by other modules
```
-DSYS_TIME_USE_TIM2=1
```

### RAM Usage
The HAL lib uses at least 20 bytes of RAM.
- 2 bytes: pseudo p register (created and used by SDCC)
- 2 bytes: pseudo pI register (replaces p in HAL interrupt functions)
- 4 bytes: reserved for HAL functions at known position (and most importantly aligned)
- 12 bytes: HAL ensures, that at least 12 bytes are reserved in the overlayed area
  - Function variables that do not call any other function will be overlayed with these bytes
  - [SDCC Compiler User Guide](https://sdcc.sourceforge.net/doc/sdccman.pdf) (3.7 Overlaying)