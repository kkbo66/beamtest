import sys
import time
sys.path.append("..") 
# from ..dmx_user_interface import dmx_broadcast
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

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x14, [0x01])
    time.sleep(0.05)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x3, [0x80])
    time.sleep(0.05)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x00c3])
    time.sleep(0.05)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x0400])
    time.sleep(0.05)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x7, [0x0000])
    time.sleep(0.05)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x8, [0x0003])
    time.sleep(0.05)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x047c])
    time.sleep(0.05)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x000c])
    time.sleep(0.05)

    dmx_reg_write("ECAL", "ECAL-FEE", 0, 1,0xb, [0x0bf0])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 0, 1,0xc, [0x00ec])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 1, 1,0xb, [0x0be0])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 1, 1,0xc, [0x00ec])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 2, 1,0xb, [0x0bdc])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 2, 1,0xc, [0x00ec])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 3, 1,0xb, [0x0bcc])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 3, 1,0xc, [0x00ec])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 4, 1,0xb, [0x0bc0])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 4, 1,0xc, [0x00ec])
    time.sleep(0.05)
    
    dmx_reg_write("ECAL", "ECAL-FEE", 0, 1,0xb, [0x099c])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 0, 1,0xc, [0x00dd])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 1, 1,0xb, [0x099c])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 1, 1,0xc, [0x00dd])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 2, 1,0xb, [0x099c])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 2, 1,0xc, [0x00dd])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 3, 1,0xb, [0x099c])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 3, 1,0xc, [0x00dd])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 4, 1,0xb, [0x099c])
    time.sleep(0.05)
    dmx_reg_write("ECAL", "ECAL-FEE", 4, 1,0xc, [0x00dd])
    time.sleep(0.05)

    # trigmode: 2
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0x2, [0x0002])
    #time.sleep(0.05)

    dmx_broadcast("ECAL", "ECAL-FEE", 1,0x2, [0x0002])
    time.sleep(0.05)
    #dmx_reg_write("ECAL", "ECAL-FEE", 4, 1,0x2, [0x0002])
    #time.sleep(0.05)

    # # calivoltage: 10
    # dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x0024])
    # time.sleep(0.05)
    # dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001c])
    # time.sleep(0.05)

    # # calivoltage: 20
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x0044])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001c])
    #time.sleep(0.05)

    # calivoltage: 40
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x0084])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001c])
    #time.sleep(0.05)

    # # calivoltage: 80
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x0108])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001c])
    #time.sleep(0.05)

    # # calivoltage: 100
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x0148])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001c])
    #time.sleep(0.05)

    # # calivoltage: 200
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x0290])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001c])
    #time.sleep(0.05)

    # # calivoltage: 400
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x0520])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001c])
    #time.sleep(0.05)

    # # calivoltage: 800
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x0a40])
    #time.sleep(0.05)
    #dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001c])
    #time.sleep(0.05)

    # # calivoltage: 1200
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x0f60])
    time.sleep(0.05)
    dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001c])
    time.sleep(0.05)

    # # calivoltage: 1600
    # dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x047c])
    # time.sleep(0.05)
    # dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001d])
    # time.sleep(0.05)

    # # calivoltage: 1800
    # dmx_broadcast("ECAL", "ECAL-FEE", 1,0xb, [0x070c])
    # time.sleep(0.05)
    # dmx_broadcast("ECAL", "ECAL-FEE", 1,0xc, [0x001d])
    # time.sleep(0.05)
