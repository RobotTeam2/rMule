# Command list

## Read Information
### format:
    info:,\r\n
    I:,\r\n
#### direction:
    Host -> Ardiono
### response:
    &$info:ch,<&$total channel number>:id0,<motor id>:id1,<motor id>:mb0,<max back>:mb1,<max back>:mf0,<max front>:mf1,<max front>:wp0,<current position>:wp1,<current position>&$


## Change setting
### format:
    setting:,\r\n
#### direction:
    Host -> Ardiono
### response:
    

## gpio control
### format:
    gpio:,\r\n
#### direction:
    Host -> Ardiono
### response:
    
## run wheel
### format:
    wheel:,\r\n
#### direction:
    Host -> Ardiono
### response:
    
## current wheel position
### format:
    wheel:vol0,<current position>
    wheel:vol1,<current position>
#### direction:
    Ardiono -> Host
### response:

