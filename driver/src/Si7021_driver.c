#include <Si7021_driver.h>
#include <i2c.h>

static const uint16_t I2C_ADDR = (0x40<<1);           // Si7021 I2C address
static const uint8_t  HEATER_CURRENT_OFFSET = 3;      // current value in mA for register value 0
static const uint8_t  HEATER_CURRENT_STEP   = 6;      // mA/LSB

static uint8_t user_register_1 = 0b00111010;
static uint8_t heater_control_register = 0b00000000;

static float process_temp_code(uint16_t temp_code);
static float process_humi_code(uint16_t humi_code);
static uint16_t convert_to_uint16(uint8_t bytes[]);
static int8_t w_reg(uint8_t value, Si7021_registers_t reg);
static int8_t r_reg(Si7021_registers_t reg);

static float process_temp_code(uint16_t temp_code)
{
  return (float)(((175.72 * temp_code) / 65536.0) - 46.85);
}

static float process_humi_code(uint16_t humi_code)
{
  float value = (float)(((125.0 * humi_code) / 65536.0) - 6.0);

  if(value < 0)
    return 0;
  else if(value > 100)
    return 100;
  else
    return (float)value;
}

static uint16_t convert_to_uint16(uint8_t bytes[])
{
  return (uint16_t)((bytes[0]<<8) | bytes[1]);
}

static int8_t r_reg(Si7021_registers_t reg)
{
  uint8_t cmd;
  uint8_t* data;

  if(reg == User_Register_1)
  {
    cmd = R_RHT_U_reg;
    data = &(user_register_1);
  }
  else if(reg == Heater_Control_Register)
  {
    cmd = R_Heater_C_reg;
    data = &(heater_control_register);
  }
  else
    return -1;

  if(HAL_OK != HAL_I2C_Mem_Read(&hi2c1, I2C_ADDR, cmd, 1, data, 1, 10000))
    return -1;
  else
    return 0;
}

static int8_t w_reg(uint8_t value, Si7021_registers_t reg)
{
  uint16_t cmd;

  if(reg == User_Register_1)
  {
    cmd = W_RHT_U_reg;
  }
  else if(reg == Heater_Control_Register)
  {
    cmd = W_Heater_C_reg;
  }
  else
    return -1;

  if(HAL_OK != HAL_I2C_Mem_Write(&hi2c1, I2C_ADDR, cmd, 1, &value, 1, 10000))
    return -1;
  else
    return 0;
}

int8_t r_single_Si7021(float* data, Si7021_measurement_type_t type)
{
  uint8_t cmd;
  uint8_t buffer[2];
  uint16_t code;

  if(type == Humidity)
    cmd = Humi_HM;
  else if(type == Temperature)
    cmd = Temp_HM;
  else
    return -1;

  if(HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR, &cmd, 1, 10000))
    return -1;

  if(HAL_OK != HAL_I2C_Master_Receive(&hi2c1, I2C_ADDR, buffer, 2, 10000))
    return -1;

  code = convert_to_uint16(buffer);

  if(type == Humidity)
    *data = process_humi_code(code);
  else if(type == Temperature)
    *data = process_temp_code(code);

  return 0;
}

int8_t r_both_Si7021(float* humidity, float* temperature)
{
  uint8_t cmd = Humi_HM;
  uint8_t buffer[2];
  uint16_t code;

  if(HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR, &cmd, 1, 10000))
    return -1;

  if(HAL_OK != HAL_I2C_Master_Receive(&hi2c1, I2C_ADDR, buffer, 2, 10000))
    return -1;

  code = convert_to_uint16(buffer);
  *humidity = process_humi_code(code);

  /* There is a temperature measurement with each RH measurement */
  cmd = Temp_AH;

  if(HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR, &cmd, 1, 10000))
    return -1;

  if(HAL_OK != HAL_I2C_Master_Receive(&hi2c1, I2C_ADDR, buffer, 2, 10000))
    return -1;

  code = convert_to_uint16(buffer);
  *temperature = process_temp_code(code);

  return 0;
}

int8_t r_firmware_rev_Si7021()
{
  uint16_t cmd = (R_Firm_rev2<<8) | R_Firm_rev1;
  uint8_t data;

  if(HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR, (uint8_t*)(&cmd), 2, 10000))
    return -1;

  if(HAL_OK != HAL_I2C_Master_Receive(&hi2c1, I2C_ADDR, &data, 1, 10000))
    return -1;

  switch(data)
  {
    case 0xFF: return 1;
    case 0x20: return 2;
    default: return -1;
  }
}

int8_t set_resolution_Si7021(Si7021_resolution_t resolution)
{
  int8_t rv;
  uint8_t temp = user_register_1;

  switch(resolution)
  {
    case H12_T14:
    {
      user_register_1 &= (uint8_t)(~(1<<RES1) & ~(1<<RES0));
      rv = w_reg(user_register_1, User_Register_1);
      break;
    }
    case H8_T12:
    {
      user_register_1 &= (uint8_t)(~(1<<RES1));
      user_register_1 |= (1<<RES0);
      rv = w_reg(user_register_1, User_Register_1);
      break;
    }
    case H10_T13:
    {
      user_register_1 &= ~(1<<RES0);
      user_register_1 |= (1<<RES1);
      rv = w_reg(user_register_1, User_Register_1);
      break;
    }
    case H11_T11:
    {
      user_register_1 |= (1<<RES1) | (1<<RES0);
      rv = w_reg(user_register_1, User_Register_1);
      break;
    }
    default: return -1;
  }

  /* in case of write error restore local copy of the register value */
  if(rv < 0)
    user_register_1 = temp;

  return rv;
}

Si7021_resolution_t r_resolution_Si7021()
{
  if(r_reg(User_Register_1) < 0)
    return -1;

  return (user_register_1 & ((1<<RES1) | (1<<RES0)));
}

int8_t set_heater_current_Si7021(uint8_t current)
{
  uint8_t reg_val = (current - HEATER_CURRENT_OFFSET)/HEATER_CURRENT_STEP;

  if(reg_val > 0x0F)
    reg_val = 0x0F;

  if(w_reg(reg_val, Heater_Control_Register) < 0)
    return -1;

  /* in case of write success update local copy of the register value */
  heater_control_register = reg_val;

  return 0;
}

int8_t r_heater_current_Si7021()
{
  if(r_reg(Heater_Control_Register) < 0)
    return -1;

  return ((heater_control_register & (0x0F)) * HEATER_CURRENT_STEP) + HEATER_CURRENT_OFFSET;
}

int8_t VDD_warning_Si7021()
{
  if(r_reg(User_Register_1) < 0)
    return -1;

  if(user_register_1 & (1<<VDDS))
    return 1;
  else
    return 0;
}

int8_t enable_heater_Si7021(uint8_t val)
{
  int8_t rv;
  uint8_t temp = user_register_1;

  if(val == 0)
  {
    user_register_1 &= ~(1<<HTRE);
    rv = w_reg(user_register_1, User_Register_1);
  }
  else
  {
    user_register_1 |= (1<<HTRE);
    rv = w_reg(user_register_1, User_Register_1);
  }

  /* in case of write error restore local copy of the register value */
  if(rv < 0)
    user_register_1 = temp;

  return rv;
}

int8_t rst_Si7021()
{
  uint8_t cmd = Si7021_Reset;

  if(HAL_OK != HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR, &cmd, 1, 10000))
    return -1;
  else
    return 0;
}

int8_t get_register(Si7021_registers_t reg, uint8_t* rv)
{
  if(r_reg(reg) < 0)
    return -1;

  if(reg == User_Register_1)
    *rv = user_register_1;
  else
    *rv = heater_control_register;

  return 0;
}
