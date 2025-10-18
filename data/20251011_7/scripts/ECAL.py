import sys
import time
sys.path.append("..") 
from ..dmx_user_interface import dmx_broadcast
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
    
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x4, [0x08])
    time.sleep(0.5)
    
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x14, [0x01])
    time.sleep(0.05)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x00c3])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x0400])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x7, [0x0000])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x8, [0x0003])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x047c])
    time.sleep(0.5)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x000c])
    time.sleep(0.5)


    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x00bc0])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x000ec])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x00099c])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x0000dd])
    #time.sleep(0.05)
