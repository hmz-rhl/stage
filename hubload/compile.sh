sudo gcc hubload.c ../lib/expander-i2c.c ../lib/MCP3202.h -o hubload -lmosquitto -lpthread -lwiringPi