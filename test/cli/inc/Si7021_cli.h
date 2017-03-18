#ifndef SI7021_CLI_H_
#define SI7021_CLI_H_

#include "stm32f4xx_hal.h"

/************************************************************************************************
* NAME :            uint8_t (*print_t)(uint8_t* buf, uint16_t len)
*
* DESCRIPTION :     General transmit function type definition.
*
* INPUTS :
*       PARAMETERS:
*            print_t              func       function pointer to the transmit function to be used
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*       RETURN :
*            Type:   uint8_t                 Error code:
*            Values:                        
*
* NOTES :    Return value is currently not in use.
*                   
*/
typedef uint8_t (*print_t)(uint8_t* buf, uint16_t len);

/************************************************************************************************
* NAME :            void Si7021_cli_init(print_t func)
*
* DESCRIPTION :     The CLI has an interface defined by a function pointer which enables the user
*                   to use any kind of physical communication.
*                   By this function the user can set a transmit function that will be used by
*                   the CLI when sending responds.
*
* INPUTS :
*       PARAMETERS:
*            print_t              func    function pointer to the transmit function to be used
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            None
*
* NOTES :   Check the definition of the print_t type to known the format of the transmit function.      
*                   
*/
void Si7021_cli_init(print_t func);

/************************************************************************************************
* NAME :            void Si7021_cli_engine(uint8_t* char_in)
*
* DESCRIPTION :     This functions takes a SINGLE byte input by reference and builds up a command until
*                   '\r' is received then it parses the command and passes it the the command handler.
*                   
*
* INPUTS :
*       PARAMETERS:
*            uint8_t*             char_in    pointer to the memory location from where the SINGLE
*                                            character (byte) input is read
*       GLOBALS :
*            None
* OUTPUTS :
*       PARAMETERS:
*            None
*       GLOBALS :
*            None
*       RETURN :
*            None
*
* NOTES :   The input is not a string but a single character which is passed by reference so
*           the CLI can change its value and set it to 0.
*                   
*/
void Si7021_cli_engine(uint8_t* char_in);

#endif /* SI7021_CLI_H_ */
