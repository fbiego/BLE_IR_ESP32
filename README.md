# BLE_IR_ESP32

Use ESP32 as a Universal IR Remote

## Android app

[`BLE_IR_Android`](https://github.com/fbiego/BLE_IR_Android)

### How to use
- Upload the firmware to an ESP32 board (pin 4 is IR out)
- Install the android app
- Pair & Connect
- Load a remote layout. Sample [`zuku.txt`](https://github.com/fbiego/BLE_IR_ESP32/blob/master/remote/NEC/zuku.txt)

## Remote layout

Each line of the layout should define a name, protocol & code separated with commas `,`

`name,protocol,code`<br>`name,protocol,code`

- name: button name
- protocol: 1 - NEC, 2 - Sony 
- code: (hex format) max 0xFFFFFFFF 8bytes

`Power,1,0xDB2410EF`<br>`,0,0x00` // button with no use
