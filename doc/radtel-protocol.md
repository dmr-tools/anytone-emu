# RadTel protocol

As used by RadTel RT-4D.

## Flow

## Packets 

### Enter program mode

#### Command 

```
 +-----+-----+-----+-----+-----+
 |   '4R'    | 05h | 10h | CRC |
 +-----+-----+-----+-----+-----+
```

#### Response
```
 +-----+
 | 06h |
 +-----+
```

#### CRC 
Just the sum modulo 256 over all other bytes.


### Read 
Reading memory. This not only reads the codeplug, but also is used to identify the radio. 

#### Request

```
 +-----+-----+-----+-----+
 | 'R' |  Address  | CRC |
 +-----+-----+-----+-----+
```

#### Response 
```
 +-----+-----+-----+-----+.....+-----+-----+
 | 'R' | Address   |   Data 1021b    | CRC |
 +-----+-----+-----+-----+.....+-----+-----+
``` 

#### Address Field
The address field is not a real memory address but rather a page number. After entering the programming mode, the page 0008h is read.


### Write 


#### Request 
```
 +-----+-----+-----+-----+.....+-----+-----+
 |9h|Se| Address   |   Data 1024b    | CRC |
 +-----+-----+-----+-----+.....+-----+-----+
```

#### Response
```
 +-----+
 | 06h |
 +-----+
```

### Leave program mode

#### Command 

```
 +-----+-----+-----+-----+-----+
 |   '4R'    | 05h | eeh | CRC |
 +-----+-----+-----+-----+-----+
```

There is no response, the radio likely reboots.
