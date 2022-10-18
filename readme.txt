Solution tank volume is DT103
Rinse tank volume is DT105

RS232 is 115200,8,1,O

(Odd parity!)

messages are in ASCII, but I'm not sure the format.  The messages that seem to have
gallons in them look like this:
%EE$RD120014

Always the same length, first 4 digits are hex, little endian, gallons. That one is 
0x0012, which is 18 gallons.

Should be easy to parse.  RS232, so requires a converter for arduino to read.

