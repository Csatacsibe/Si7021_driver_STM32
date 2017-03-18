#ifndef SI7021_H_
#define SI7021_H_

#include "stm32f4xx_hal.h"

#define RES0 0
#define RES1 7
#define VDDS 6
#define HTRE 2

typedef enum Si7021_commands
{
  Humi_HM        = 0xE5, // Measure Relative Humidity, Hold Master Mode
  Humi_NHM       = 0xF5, // Measure Relative Humidity, No Hold Master Mode
  Temp_HM        = 0xE3, // Measure Temperature, Hold Master Mode
  Temp_NHM       = 0xF3, // Measure Temperature, No Hold Master Mode
  Temp_AH        = 0xE0, // Read Temperature Value from Previous RH Measurement
  Si7021_Reset   = 0xFE, // Reset
  W_RHT_U_reg    = 0xE6, // Write RH/T User Register 1
  R_RHT_U_reg    = 0xE7, // Read RH/T User Register 1
  W_Heater_C_reg = 0x51, // Write Heater Control Register
  R_Heater_C_reg = 0x11, // Read Heater Control Register
  R_ID_Byte11    = 0xFA, // Read Electronic ID 1st Byte, first part
  R_ID_Byte12    = 0x0F, // Read Electronic ID 1st Byte, second part
  R_ID_Byte21    = 0xFC, // Read Electronic ID 2nd Byte, first part
  R_ID_Byte22    = 0xC9, // Read Electronic ID 2nd Byte, second part
  R_Firm_rev1    = 0x84, // Read Firmware Revision, first part
  R_Firm_rev2    = 0xB8  // Read Firmware Revision, second part
}Si7021_commands_t;

typedef enum Si7021_measurement_type
{
  Humidity,
  Temperature
}Si7021_measurement_type_t;

typedef enum Si7021_registers
{
  User_Register_1,
  Heater_Control_Register
}Si7021_registers_t;

typedef enum Si7021_resolution
{
  H12_T14 = 0x00,
  H8_T12  = 0x01,
  H10_T13 = 0x80,
  H11_T11 = 0x81
}Si7021_resolution_t;

/************************************************************************************************
* NAME :            int8_t r_firmware_rev_Si7021(void)
*
* DESCRIPTION :     Reads the Si7021 internal firmware revision.
*
* INPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  1                     firmware revision is 1.0
*                     2                     firmware revision is 2.0
*                    -1                     I2C error or invalid data
*
* NOTES :           
*                   
*/
int8_t r_firmware_rev_Si7021(void);

/************************************************************************************************
* NAME :            int8_t VDD_warning_Si7021(void)
*
* DESCRIPTION :     The minimum recommended operating voltage is 1.9 V. A transition
*                   of the VDD status bit from 0 to 1 indicates that VDD is
*                   between 1.8 V and 1.9 V. If the VDD drops below 1.8 V, the
*                   device will no longer operate correctly.
*                   The function reads the status bit and returns the VDD status.
*
* INPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  0                     no VDD warning
*                     1                     VDD warning is present
*                    -1                     I2C error
*
* NOTES :           
*                   
*/
int8_t VDD_warning_Si7021(void);

/************************************************************************************************
* NAME :            int8_t r_single_Si7021(float* data, Si7021_measurement_type_t type)
*
* DESCRIPTION :     Initiates a measurement defined by the 'type' parameter and reads
*                   back its result. It can be either a humidity or a temperature
*                   measurement.
*
* INPUTS :
*       PARAMETERS:
*            Si7021_measurement_type_t      type      type of measurement to be done
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            float*                         data      pointer to memory location where the
*                                                     measurement result will be stored
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  0                     OK
*                    -1                     I2C error or invalid measurement type parameter
*
* NOTES :          The function uses the Hold Master Mode I2C command to request the measurement
*                  and to read back the result.
*                   
*/
int8_t r_single_Si7021(float* data, Si7021_measurement_type_t type);

/************************************************************************************************
* NAME :            int8_t r_both_Si7021(float* humidity, float* temperature)
*
* DESCRIPTION :     Initiates a humidity measurement and -as each time a relative humidity measurement
*                   is made a temperature measurement is also made- it returns both the humidity and 
*                   temperature measurement results.
*
* INPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            float*                         humidity      pointer to memory location where the
*                                                         humidity measurement result will be stored
*            float*                         temperature   pointer to memory location where the
*                                                         temperature measurement result will be stored
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  0                     OK
*                    -1                     I2C error
*
* NOTES :          The function uses the Hold Master Mode I2C command to request the humidity 
*                  measurement and to read back the result.
*                   
*/
int8_t r_both_Si7021(float* humidity, float* temperature);

/************************************************************************************************
* NAME :            int8_t set_resolution_Si7021(Si7021_resolution_t resolution)
*
* DESCRIPTION :     Sets the relative humidity and temperature measurements' resolution to
*                   the one defined by the 'resolution' parameter.
*
* INPUTS :
*       PARAMETERS:
*            Si7021_resolution_t            resolution    resolution value to be set
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  0                     OK
*                    -1                     I2C error or invalid resolution parameter
*
* NOTES :          
*                   
*/
int8_t set_resolution_Si7021(Si7021_resolution_t resolution);


/************************************************************************************************
* NAME :            Si7021_resolution_t r_resolution_Si7021(void)
*
* DESCRIPTION :     Reads back and returns the current relative humidity
*                   and temperature measurements' resolution.
*
* INPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            Type:   Si7021_resolution_t    Error code:
*            Values: H12_T14 = 0x00         RH 12 bit, Temp 14 bit
*                    H8_T12  = 0x01         RH  8 bit, Temp 12 bit
*                    H10_T13 = 0x80         RH 10 bit, Temp 13 bit
*                    H11_T11 = 0x81         RH 11 bit, Temp 11 bit
*                    -1                     I2C error or invalid resolution parameter
*
* NOTES :          
*                   
*/
Si7021_resolution_t r_resolution_Si7021(void);

/************************************************************************************************
* NAME :            int8_t set_heater_current_Si7021(uint8_t current)
*
* DESCRIPTION :     Sets the current of the on-chip heater to the value passed
*                   by 'current' parameter. Values are in mA.
*
* INPUTS :
*       PARAMETERS:
*            uint8_t                        current       current in mA
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  0                     OK
*                    -1                     I2C error
*
* NOTES :          Values are in mA and VDD assumed to be 3.3 V.
*                  Lowest value is 3 mA that can be increased in ~6 mA steps up to ~94 mA. 
*                  Values above maximum sets the heater current to maximum. 
*                  Generally values are always rounded down. For example a
*                  parameter value of 14 mA will result the heater current to be set to
*                  9 mA even though the closest valid step is 15 mA.
*/
int8_t set_heater_current_Si7021(uint8_t current);

/************************************************************************************************
* NAME :            int8_t r_heater_current_Si7021(void)
*
* DESCRIPTION :     Reads back and returns in mA the current setting of the on-chip heater.
*
* INPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  <current>             current value in mA (3 mA < [x] < 94 mA)
*                    -1                     I2C error
*
* NOTES :          Values are in mA and VDD assumed to be 3.3 V.
*/
int8_t r_heater_current_Si7021(void);

/************************************************************************************************
* NAME :            int8_t enable_heater_Si7021(uint8_t val)
*
* DESCRIPTION :     Enables or disables the on-chip heater. 
*
* INPUTS :
*       PARAMETERS:
*            uint8_t                        val      value of '0' means disable
*                                                    every other means enable
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  0                     OK
*                    -1                     I2C error
*
* NOTES :
*/
int8_t enable_heater_Si7021(uint8_t val);

/************************************************************************************************
* NAME :            int8_t rst_Si7021(void)
*
* DESCRIPTION :     Initiates a Si7021 software reset by the appropriate command. 
*
* INPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  0                     OK
*                    -1                     I2C error
*
* NOTES :
*/
int8_t rst_Si7021(void);

/************************************************************************************************
* NAME :            int8_t get_register(Si7021_registers_t reg, uint8_t* rv)
*
* DESCRIPTION :     Returns the value of the selected register. 
*
* INPUTS :
*       PARAMETERS:
*            Si7021_registers_t             reg        register to be queried
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            uint8_t*                       rv         pointer to the mermory location
*                                                      where the register value will be stored
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int8_t                 Error code:
*            Values:  0                     OK
*                    -1                     I2C error or invalid register parameter
*
* NOTES :
*/
int8_t get_register(Si7021_registers_t reg, uint8_t* rv);

#endif /* SI7021_H_ */
