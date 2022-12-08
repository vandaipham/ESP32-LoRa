## LoRa-transmitter
Packet format:
{"h": DoAm,"t":NhietDo,"lux":lux}

Get commands:
* AutoMode (true or false) - 0x01 or 0x02.
* Mayphunsuong (true or false) - 0x03 or 0x04.
* Maysuoi (true or false) - 0x05 or 0x06
* Mayquat (true or false) - 0x07 or 0x08
* Loa (true or false) - 0x09 or 0x0A
* keomanchelen vs keomanchexuong 
* Donglen == "true" & Haxuong == "false" - 0x0B
* Donglen == "false" & Haxuong == "false" - 0x0C
* Donglen == "false" & Haxuong == "true" - 0x0D