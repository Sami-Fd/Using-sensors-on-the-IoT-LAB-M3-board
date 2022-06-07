APPLICATION = sensors

BOARD ?= iotlab-m3

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ztimer_msec
USEMODULE += xtimer

# Add the required modules here
USEMODULE += lps331ap
USEMODULE += isl29020
USEMODULE += lsm303dlhc
USEMODULE += l3g4200d

RIOTBASE ?= $(CURDIR)/../../RIOT

include $(RIOTBASE)/Makefile.include
