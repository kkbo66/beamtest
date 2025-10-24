import sys
import time
sys.path.append("..") 
#from ..dmx_user_interface import dmx_broadcast
from ..dmx_user_interface import dmx_reg_write, dmx_broadcast
from config.log_config import logger
# from config.config import data_path
# from ..services.system import system_instance
# from ..services.control import control_instance
import struct

def _str_to_uint32_array(input_string):
    """
    将字符串转换为 uint32 数组
    每个字符占用 4 个字节
    """
    uint32_array = []
    for char in input_string:
        uint32 = struct.unpack('I', char.encode('utf-32le'))[0]
        uint32_array.append(uint32)
    return uint32_array

def on_initial_power_up():
    logger.debug("This is ECAL on_initial_power_up")

def on_incremental_power_up():
    logger.debug("This is ECAL on_incremental_power_up")

def on_parameter_setup():
    logger.debug("This is ECAL on_parameter_setup")
   
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x2, [0x0000])
    time.sleep(0.5)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x3, [0x80])
    time.sleep(0.5)

    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0x4, [0x08])
    #time.sleep(0.5)
    
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x14, [0x01])
    time.sleep(0.05)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x19, [0x0000])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x19, [0x0001])
    time.sleep(0.5)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x00c3])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x0400])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x00c3])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x0400])
    time.sleep(0.5)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x7, [0x0000])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x8, [0x0003])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x7, [0x0000])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x8, [0x0003])
    time.sleep(0.5)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x047c])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x000c])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x047c])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x000c])
    time.sleep(0.5)

    dmx_reg_write("ECAL", "ECAL-FEE", 0, 1,0xb, [0x0bf0])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 0, 1,0xc, [0x00ec])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 1, 1,0xb, [0x0be0])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 1, 1,0xc, [0x00ec])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 2, 1,0xb, [0x0bdc])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 2, 1,0xc, [0x00ec])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 3, 1,0xb, [0x0bcc])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 3, 1,0xc, [0x00ec])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 4, 1,0xb, [0x0bc0])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 4, 1,0xc, [0x00ec])
    time.sleep(0.5)
    
    dmx_reg_write("ECAL", "ECAL-FEE", 0, 1,0xb, [0x099c])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 0, 1,0xc, [0x00dd])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 1, 1,0xb, [0x099c])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 1, 1,0xc, [0x00dd])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 2, 1,0xb, [0x099c])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 2, 1,0xc, [0x00dd])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 3, 1,0xb, [0x099c])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 3, 1,0xc, [0x00dd])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 4, 1,0xb, [0x099c])
    time.sleep(0.5)
    dmx_reg_write("ECAL", "ECAL-FEE", 4, 1,0xc, [0x00dd])
    time.sleep(0.5)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x11, [0x01])
    time.sleep(0.2)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x11, [0x00])
    time.sleep(0.2)

    # for tq
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x12, [0x0])
    time.sleep(0.2)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x12, [0x1])
    time.sleep(0.2)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x11, [0x01])
    time.sleep(0.2)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x11, [0x00])
    time.sleep(0.2)

    dmx_reg_write("ECAL", "ECAL-FEE", 1, 1,0x15, [0x96])
    time.sleep(0.2)
    dmx_reg_write("ECAL", "ECAL-FEE", 2, 1,0x15, [0x96])
    time.sleep(0.2)
    dmx_reg_write("ECAL", "ECAL-FEE", 3, 1,0x15, [0x96])
    time.sleep(0.2)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x16, [0x09fb09fb])
    time.sleep(0.2)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x17, [0x09fb09fb])
    time.sleep(0.2)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x18, [0x09fb09fb])
    time.sleep(0.2)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x14, [0x03])
    time.sleep(0.5)

    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x00bc0])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x000ec])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x00099c])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x0000dd])
    #time.sleep(0.05)
