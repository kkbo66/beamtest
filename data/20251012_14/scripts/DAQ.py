import sys
sys.path.append("..") 
from ..dmx_oper import dmx_oper_instance
from ..dmx_user_interface import dmx_reg_write, dmx_broadcast
#from ..dmx_user_interface import dmx_reg_write
from config.log_config import logger
from config.config import data_path
from ..services.system import system_instance
import struct

from config.config import MODE
eventlet_enable = True if MODE.get()=="SERVER_MODE" else False
if eventlet_enable:
    # import eventlet.green.threading
    from eventlet import sleep
else:
    # import threading
    from time import sleep

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
    logger.debug("This is DAQ on_initial_power_up")

def on_incremental_power_up():
    logger.debug("This is DAQ on_incremental_power_up")

def on_parameter_setup():
    logger.debug("This is DAQ on_parameter_setup")
    dmx_reg_write("DAQ", "PCIe", 0x1, 0, 0x8, [0x03])
    #dmx_reg_write("DAQ", "PXI", 0x0, 0, 0x18, [0x80000000])

def on_sync_setup():
    logger.debug("This is DAQ on_sync_setup")

def on_start():
    logger.info("DAQ分系统执行start")
    # 注册所有定时器
    dmx_oper_instance.timer_register_all()
    # 检查状态读取定时器是否正在运行，如果不在运行则启动
    timer_name = "on_timer_status_read_DAQ"
    if timer_name in dmx_oper_instance.current_timer:
        if dmx_oper_instance.current_timer[timer_name]["status"] != "running":
            sleep(0)
            dmx_oper_instance.start_timer(timer_name)
    # 发送数据路径
    logger.info(f"发送数据路径：{data_path.get_full_data_path()}")
    path_uint32_array = _str_to_uint32_array(data_path.get_full_data_path())
    save_das_id_list = [0]
    save_ln_id_list = [8,9,10,11]
    for save_das_id in save_das_id_list:
        for save_ln_id in save_ln_id_list:
            dmx_reg_write("DAQ", "DAS", save_das_id, save_ln_id, 0x0, path_uint32_array)
    # dmx_reg_write("DAQ", "DAS", 1, 15, 0x0, path_uint32_array)
    # dmx_reg_write("DAQ", "DAS", 0, 16, 0x0, path_uint32_array)
    # 记录运行信息
    from ..services.workmode import workmode_instance
    system_instance.new_run_info(mode=workmode_instance.current_workmode, data_dir=data_path.get_data_path())
    # 发送start命令
    dmx_broadcast("DAQ", "OCS", 0, 0x10, [0x01])
    dmx_broadcast("DAQ", "DAS", 0, 0x10, [0x01])
    sleep(1)
    dmx_broadcast("ECAL", "ECAL-FEE", 0, 0x10, [0x01])
    dmx_broadcast("TRIG", "TRIG", 0, 0x10, [0x01])
    dmx_broadcast("TRIG", "TRIG", 0, 0x10, [0x00])
    dmx_reg_write("PIDB", "PIDB-FEE", 0x1, 0, 0x10, [0x01])
    dmx_reg_write("PIDB", "PIDB-FEE", 0x1, 0, 0x10, [0x00])
    dmx_broadcast("PIDE", "PIDE-FEE", 0, 0x10, [0x01])
    dmx_broadcast("PIDE", "PIDE-FEE", 0, 0x10, [0x00])

def on_stop():
    logger.debug("DAQ分系统执行stop")
    # 若状态读取定时器正在运行，则暂停
    timer_name = "on_timer_status_read_DAQ"
    if timer_name in dmx_oper_instance.current_timer:
        if dmx_oper_instance.current_timer[timer_name]["status"] != "paused":
            dmx_oper_instance.pause_timer(timer_name)
    #dmx_reg_write("TRIG", "MTM", 0x0, 0, 0x10, [0x02])
    #dmx_reg_write("TRIG", "MTM", 0x0, 0, 0x10, [0x00])
    # dmx_broadcast("ECAL", "ECAL-FEE", 0, 0x10, [0x02])
    #dmx_broadcast("ECAL", "ECAL-FEE", 0, 0x10, [0x00])
    dmx_broadcast("ECAL", "ECAL-FEE", 0, 0x10, [0x00])
    dmx_broadcast("TRIG", "TRIG", 0, 0x10, [0x02])
    dmx_broadcast("TRIG", "TRIG", 0, 0x10, [0x00])
    dmx_reg_write("PIDB", "PIDB-FEE", 0x1, 0, 0x10, [0x02])
    dmx_reg_write("PIDB", "PIDB-FEE", 0x1, 0, 0x10, [0x00])
    dmx_broadcast("PIDE", "PIDE-FEE", 0, 0x10, [0x02])
    dmx_broadcast("PIDE", "PIDE-FEE", 0, 0x10, [0x00])
    sleep(2)
    dmx_broadcast("DAQ", "OCS", 0, 0x10, [0x02])
    dmx_broadcast("DAQ", "DAS", 0, 0x10, [0x02])
    # 记录运行信息
    system_instance.end_run_info()
    from ..services.control import control_instance
    control_instance.save_topology_to_data_path()
    from ..services.workmode import workmode_instance
    workmode_instance.save_workmode_to_data_path()
    dmx_oper_instance.copy_scripts()

def on_DAQ_reset():
    logger.debug("DAQ分系统执行DAQ reset")
    #dmx_broadcast("ECAL", "ECAL-FEE", 0, 0x10, [0x10])
    # dmx_broadcast("ECAL", "ECAL-FEE", 0, 0x10, [0x0])
    dmx_broadcast("DAQ", "PXI", 0, 0x10, [0x10])
    dmx_broadcast("DAQ", "PCIe", 0, 0x10, [0x10])
    dmx_broadcast("DAQ", "DAS", 0, 0x10, [0x10])
    dmx_broadcast("DAQ", "OCS", 0, 0x10, [0x10])

def on_clear():
    logger.debug("DAQ分系统执行clear")
    dmx_broadcast("TRIG", "TRIG", 1, 0x10, [0xAAFFFF])

def on_emergent_msg():
    logger.debug("This is DAQ on_emergent_msg")

@dmx_oper_instance.register_timer(interval=1, info="Status Read Timer")
def on_timer_status_read():
    logger.debug("This is DAQ on_timer_status_read thread")
    dmx_broadcast("DAQ", "DAS", 2, 0x10, [0x8])

