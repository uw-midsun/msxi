"""
A simple Python script to build a lookup-table for the NTCLE100E3103HB0
NTC thermistors and an ADC device

Steinhart-Hart equation [1]:
T(R) = 1 / (a + (b * log(R / R_25)) + (c * (log(R / R_25)^2)) + (d * (log(R / R_25)^3)))

Voltage Divider equation [2]:
V_o = (R * V_cc) / (R + R_ref)

ADC equation [3]:
ADC reading = (V_o * 2 ^ resolution) / V_cc

Using [2] and [3], you can solve for R as a function of the ADC reading [4]
R = (R_ref * ADC reading) / ((2 ^ adc_res - 1) - ADC reading)

Plug [4] into [1] and you have T as a function of your ADC reading in K
"""
from math import log

# taken from datasheet
a = 0.00335401643468053
b = 0.00025698501802
c = 0.0000026201306709
d = 0.000000063830907998

# reference resistance of the thermistor at 25 degrees C
R_25  = 10000
# reference resistor used in voltage divider equation
R_ref = 10000
# ADC resolution in bits
adc_res = 12

def KtoC(k):
    return k + -273.15

def adc_to_temp(adc_reading):
    R = (R_ref * adc_reading) / ((2 ** adc_res - 1) - adc_reading)
    T = 1 / (a + (b * log(R / R_25)) + (c * (log(R / R_25) ** 2)) + (d * (log(R / R_25) ** 3)))
    return KtoC(T)

i = 0
for x in range(0, 2 ** adc_res):
    try:
        #print ("%d" %(adc_to_temp(x)), end = ',')
        print ("%d" %(adc_to_temp(x)))
    except:
        if x == 0:
            #print ("%d" %(adc_to_temp(x + 1)), end = ',')
            print ("%d" %(adc_to_temp(x + 1)))
        else:
            #print ("%d" %(adc_to_temp(x - 1)), end = ',')
            print ("%d" %(adc_to_temp(x - 1)))
    i = i + 1
    #if i % 10 == 0:
        #print ("")
