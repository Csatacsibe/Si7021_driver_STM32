#include "Si7021_cli.h"
#include "Si7021_driver.h"
#include "stdio.h"
#include "string.h"

static print_t print = NULL;
static uint8_t message[500];

static void printf_binary(uint8_t value);

static int8_t show_humidity(void);
static int8_t show_temperature(void);
static int8_t show_humidity_n_temperature(void);

static int8_t show_user_reg1(void);
static int8_t show_heater_control_reg(void);

static int8_t show_firmware_rev(void);
static int8_t query_vdd_warning(void);

static int8_t reset(void);

static int8_t show_heater_current(void);
static int8_t set_heater_current(uint8_t param);
static int8_t enable_heater(uint8_t param);

static int8_t show_measurement_resolutions(void);
static int8_t set_measurement_resolutions(uint8_t param);

static int8_t show_cli_usage_help(void);

static void cli_command_handler(uint8_t command_code, uint8_t param);

static void printf_binary(uint8_t value)
{
  uint8_t i = 0;
  uint8_t chunk[10];

  for(i = 0; i < 8; i++)
  {
    HAL_Delay(2);

    sprintf((char*)chunk, " %3d |", (value)&(1<<(7 - i))? 1 : 0);
    print(chunk, strlen((char*)chunk));
  }
  HAL_Delay(2);
}

static int8_t show_humidity()
{
  float humidity = 0;
  int8_t rv;

  rv = r_single_Si7021(&humidity, Humidity);

  if(rv >= 0)
  {
    sprintf((char*)message, "Humidity:%0.f%%\r\n", humidity);
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t show_temperature()
{
  float temperature = 0;
  int8_t rv;

  rv = r_single_Si7021(&temperature, Temperature);

  if(rv >= 0)
  {
    sprintf((char*)message, "Temperature: %.1f C\r\n", temperature);
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t show_humidity_n_temperature()
{
  float humidity = 0, temperature = 0;
  int8_t rv;

  rv = r_both_Si7021(&humidity, &temperature);

  if(rv >= 0)
  {
    sprintf((char*)message,
        "Humidity: %0.f%% Temperature: %.1f C\r\n", humidity, temperature);
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t show_user_reg1()
{
  uint8_t reg, rv;
  rv = get_register(User_Register_1, &reg);

  if(rv >= 0)
  {
    sprintf((char*)message,
        "\r\nUser Register 1: 0x%02x\r\n"
        "------------------------------------------------\r\n"
        " RES1| VDDS| RSVD| RSVD| RSVD| HTRE| RSVD| RES0|\r\n"
        "------------------------------------------------\r\n", reg);
    print(message, strlen((char*)message));

    printf_binary(reg);

    sprintf((char*)message,
        "\r\n------------------------------------------------\r\n");
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t show_heater_control_reg()
{
  uint8_t reg, rv;
  rv = get_register(Heater_Control_Register, &reg);

  if(rv >= 0)
  {
    sprintf((char*)message,
        "\r\nHeater Control Register: 0x%02x\r\n"
        "------------------------------------------------\r\n"
        " RSVD| RSVD| RSVD| RSVD| HTR3| HTR2| HTR1| HTR0|\r\n"
        "------------------------------------------------\r\n", reg);
    print(message, strlen((char*)message));

    printf_binary(reg);

    sprintf((char*)message,
        "\r\n------------------------------------------------\r\n");
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t show_firmware_rev()
{
  int8_t rv = r_firmware_rev_Si7021();

  if(rv >= 0)
  {
    sprintf((char*)message, "Si7021 firmware rev: %d\r\n", rv);
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t query_vdd_warning()
{
  int8_t rv = VDD_warning_Si7021();

  if(rv >= 0)
  {
    sprintf((char*)message, "VDD warning: %d\r\n", rv);
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t reset()
{
  int8_t rv = rst_Si7021();

  if(rv >= 0)
  {
    sprintf((char*)message, "Si7021 reset successful!\r\n");
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t show_heater_current()
{
  int8_t rv = r_heater_current_Si7021();

  if(rv >= 0)
  {
    sprintf((char*)message, "Heater current: %d mA (assuming VDD is 3.3 V )\r\n", rv);
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t set_heater_current(uint8_t param)
{
  int8_t rv = set_heater_current_Si7021(param);

  if(rv >= 0)
  {
    sprintf((char*)message,
        "Heater current changed successfully to the closest value to %d mA\r\n", param);
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t enable_heater(uint8_t param)
{
  int8_t rv = enable_heater_Si7021(param);

  if(rv >= 0)
  {
    if(param)
      sprintf((char*)message, "On-chip heater enabled successfully!\r\n");
    else
      sprintf((char*)message, "On-chip heater disabled successfully!\r\n");

    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t show_measurement_resolutions()
{
  int8_t rv = r_resolution_Si7021();

  if(rv == -1)
    return rv;

  sprintf((char*)message, "Measurement resolutions:\r\n");
  print(message, strlen((char*)message));

  HAL_Delay(2);

  switch((Si7021_resolution_t)rv)
  {
  case H12_T14:
    sprintf((char*)message, "RH: 12 bit Temp: 14 bit\r\n");
    break;
  case H11_T11:
    sprintf((char*)message, "RH: 11 bit Temp: 11 bit\r\n");
    break;
  case H10_T13:
    sprintf((char*)message, "RH: 10 bit Temp: 13 bit\r\n");
    break;
  case H8_T12:
    sprintf((char*)message, "RH:  8 bit Temp: 12 bit\r\n");
    break;
  default:
    return rv;
  }

  print(message, strlen((char*)message));

  return rv;
}

static int8_t set_measurement_resolutions(uint8_t param)
{
  Si7021_resolution_t type = H12_T14;
  int8_t rv;

  switch(param)
  {
  case 0:
    sprintf((char*)message, "Setting measurement resolutions to RH: 12 bit Temp: 14 bit\r\n");
    type = H12_T14;
    break;
  case 1:
    sprintf((char*)message, "Setting measurement resolutions to RH: 11 bit Temp: 11 bit\r\n");
    type = H11_T11;
    break;
  case 2:
    sprintf((char*)message, "Setting measurement resolutions to RH: 10 bit Temp: 13 bit\r\n");
    type = H10_T13;
    break;
  case 3:
    sprintf((char*)message, "Setting measurement resolutions to RH:  8 bit Temp: 12 bit\r\n");
    type = H8_T12;
    break;
  default:
    sprintf((char*)message, "Invalid option!\r\n");
    break;
  }

  print(message, strlen((char*)message));

  rv = set_resolution_Si7021(type);

  if(rv >= 0)
  {
    sprintf((char*)message, "Measurement resolutions successfully changed!\r\n");
    print(message, strlen((char*)message));
  }

  return rv;
}

static int8_t show_cli_usage_help()
{
  sprintf((char*)message,
      "Available commands:\n\r"
      "h: get humidity\n\r"
      "t: get temperature\n\r"
      "b: get humidity and temperature\n\r"
      "u: read User Register 1\n\r"
      "e: read Heater Control Register\n\r"
      "f: read firmware revision\n\r"
      "v: get VDD warning status\n\r"
      "c: read heater current\n\r"
      "m: read measurement resolution\n\r"
  );
  print(message, strlen((char*)message));

  HAL_Delay(2);

  sprintf((char*)message,
      "n <current>: set heater current in mA\n\r"
      "a <option>: set measurement resolution\n\r"
      "            0: RH 12 bit, Temp 14 bit\r\n"
      "            1: RH 11 bit, Temp 11 bit\r\n"
      "            2: RH 10 bit, Temp 13 bit\r\n"
      "            3: RH  8 bit, Temp 12 bit\r\n"
      "l <enable>: enable or disable on-chip heater\r\n"
      "            0: disable\r\n"
      "            1: enable\r\n"
      "r: reset Si7021\n\r"
      );
  print(message, strlen((char*)message));

  return 0;
}

static void cli_command_handler(uint8_t command_code, uint8_t param)
{
  int8_t rv = 0;

  if(command_code == 'h')
  {
    rv = show_humidity();
  }
  else if(command_code == 't')
  {
    rv = show_temperature();
  }
  else if(command_code == 'b')
  {
    rv = show_humidity_n_temperature();
  }
  else if(command_code == 'u')
  {
    rv = show_user_reg1();
  }
  else if(command_code == 'e')
  {
    rv = show_heater_control_reg();
  }
  else if(command_code == 'f')
  {
    rv = show_firmware_rev();
  }
  else if(command_code == 'v')
  {
    rv = query_vdd_warning();
  }
  else if(command_code == 'r')
  {
    rv = reset();
  }
  else if(command_code == 'c')
  {
    rv = show_heater_current();
  }
  else if(command_code == 'n')
  {
    rv = set_heater_current(param);
  }
  else if(command_code == 'l')
  {
    rv = enable_heater(param);
  }
  else if(command_code == 'm')
  {
    rv = show_measurement_resolutions();
  }
  else if(command_code == 'a')
  {
    rv = set_measurement_resolutions(param);
  }
  else if(command_code == 0)
  {

  }
  else
  {
    rv = show_cli_usage_help();
  }

  if(rv < 0)
  {
    sprintf((char*)message, "Operation failed!\r\n");
    print(message, strlen((char*)message));
  }

}

void Si7021_cli_init(print_t func)
{
  if(print == NULL)
  {
    print = func;
  }
}

/*
*  Note that the input is a single byte passed by reference 
*  to be able to clear it.
*/
void Si7021_cli_engine(uint8_t* char_in)
{
  static uint8_t input_buffer[20];
  static uint8_t input_index = 0;

  uint8_t command_code = 0, param = 0, ready = 0;

  if(*char_in != 0)
  {
    if(*char_in != '\r')
    {
      input_buffer[input_index++] = *char_in;
    }
    else
    {
      uint8_t rv = 0;
      input_buffer[input_index] = 0;
      input_index = 0;

      rv = sscanf(input_buffer, "%c %d", &command_code, &param);

      if(rv == 2)
        ready = 1;
      else if(rv == 1)
      {
        ready = 1;
        param = 0;
      }
    }
    
    /* clear input */
    *char_in = 0;

    if(ready)
    {
      cli_command_handler(command_code, param);
    }
  }
}
