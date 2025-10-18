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
    logger.debug("This is trig on_initial_power_up")

def on_incremental_power_up():
    logger.debug("This is trig on_incremental_power_up")

def on_parameter_setup():
    logger.debug("This is trig on_parameter_setup")
    
    dmx_broadcast("TRIG", "TRIG", 1,0xEA, [0x5AFFFF])
    time.sleep(0.1)
    
    #dmx_broadcast("TRIG", "TRIG", 1,0xBC, [0x8002FF])#5k
    #time.sleep(0.05)

    #dmx_broadcast("TRIG", "TRIG", 1,0xBC, [0xC001FF])#1k
    #time.sleep(0.05)

    dmx_broadcast("TRIG", "TRIG", 1,0xBC, [0xC002FF])#500
    time.sleep(0.05)
  
    #dmx_broadcast("TRIG", "TRIG", 1,0xBC, [0xC00AFF])#100
    #time.sleep(0.05)

    #dmx_broadcast("TRIG", "TRIG", 1,0xBC, [0xC032FF])#20
    #time.sleep(0.05)

    dmx_broadcast("TRIG", "TRIG", 1,0xDC, [0xFF00C8])#200thr
    time.sleep(0.05)

    dmx_broadcast("TRIG", "TRIG", 1,0xBA, [0xFFFFFF])
    time.sleep(0.1)


