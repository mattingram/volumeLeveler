adc=require("adc")

-- Map PCB for Arduino to NodeMCU/Lua
GPIO0 = 3
GPIO2 = 4
GPIO3 = 9
GPIO4 = 2 -- may need to swap
GPIO5 = 1
GPIO9 = 11
GPIO10 = 12
GPIO12 = 6
GPIO13 = 7
GPIO14 = 5
GPIO15 = 8
GPIO16 = 0
SCL = GPIO5
SDA = GPIO4
REDLED = GPIO0
BLUELED = GPIO2
ON = gpio.LOW
OFF = gpio.HIGH

--gpio.mode(REDLED, gpio.OUTPUT)
gpio.mode(BLUELED, gpio.OUTPUT)
gpio.mode(GPIO12, gpio.INPUT)

function SetLED(led, state)
  gpio.write(led, state)
end

initialReading = adc.read(0)
threshold = 5

print("initialReading=", initialReading)
print("threshold=", threshold)

while 1 do
  --micDigital = gpio.read(GPIO12)
  micAnalog = adc.read(0)
  
  -- if (micAnalog < initialReading - threshold) then
  --   print("Below threshold=", micAnalog)
  -- end

  if (micAnalog > initialReading + threshold) then
	print("Above threshold=", micAnalog)
	SetLED(BLUELED, ON)
  else
  	SetLED(BLUELED, OFF)
  end

  -- if (micVal == OFF) then
  -- 	SetLED(REDLED, OFF)
  -- else
  -- 	SetLED(REDLED, ON)
  -- end

 --SetLED(BLUELED, ON)
 --tmr.delay(200000)
 --SetLED(BLUELED, OFF)
 tmr.delay(1000)
end