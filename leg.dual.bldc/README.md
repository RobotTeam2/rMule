# High Level Command
## summary
   relative x posion of one leg in mm  


## read board info
### format:
    who:\r\n
#### direction:
    Host -> Arduino
### response:
    who:ch0,<id number>:ch1,<id number>\r\n   
## move leg  
### format:
    legM:id,<id number>:xmm,<target position in mm>:payload,<0,1>\r\n  
### format short:
    M:i,<id number>:x,<target position in mm>:p,<0,1>\r\n  
#### direction:  
    Host -> Arduino  
### response:
    legM:id,<id number>:\r\n  
## read leg position
### format:
    legG:id,<id number>\r\n  
#### direction:
    Host -> Arduino
### response:
    legG:id,<id number>:xmm,<current position in mm>\r\n  
## log level
### format:
    log:level,<level 0~>\r\n  
    degug:level,<level 0~>\r\n  
#### direction:
    Host -> Arduino
### response:   
            

# Low Level Command

## Read Information
### format:
    info:,\r\n
    I:,\r\n
#### direction:
    Host -> Arduino
### response:
    &$info:ch,<&$total channel number>:id0,<motor id>:id1,<motor id>:mb0,<max back>:mb1,<max back>:mf0,<max front>:mf1,<max front>:wp0,<current position>:wp1,<current position>&$


## Change setting
### format:
    setting:id0,<id>:id1,<id>:mb0,<max back>:mb1,<max back>:mf0,<max front>:mf1,<max front>\r\n
#### direction:
    Host -> Arduino
### response:
    

## gpio control (TBD)
### format:
    gpio:,\r\n
#### direction:
    Host -> Arduino
### response:
    
## run wheel
### format:
    wheel:vol0,<target position>:vol1,<target position>\r\n
#### direction:
    Host -> Arduino
### response:
    
## current wheel position
### format:
    wheel:vol0,<current position>
    wheel:vol1,<current position>
#### direction:
    Arduino -> Host
### response:
    


