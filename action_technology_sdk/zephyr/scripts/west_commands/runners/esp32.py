# Copyright (c) 2017 Linaro Limited.
# Copyright (c) 2021 Espressif Systems (Shanghai) Co., Ltd.
#
# SPDX-License-Identifier: Apache-2.0

'''Runner for flashing ESP32 devices with esptool/espidf.'''

from os import path

from runners.core import ZephyrBinaryRunner, RunnerCaps

import os
import sys

class Esp32BinaryRunner(ZephyrBinaryRunner):
    '''Runner front-end for espidf.'''

    def __init__(self, cfg, device, boot_address, part_table_address,
                 app_address, baud=921600, flash_size='detect',
                 flash_freq='40m', flash_mode='dio', espidf='espidf',
                 bootloader_bin=None, partition_table_bin=None):
        super().__init__(cfg)
        self.elf = cfg.elf_file
        self.device = device
        self.boot_address = boot_address
        self.part_table_address = part_table_address
        self.app_address = app_address
        self.baud = baud
        self.flash_size = flash_size
        self.flash_freq = flash_freq
        self.flash_mode = flash_mode
        self.espidf = espidf
        self.bootloader_bin = bootloader_bin
        self.partition_table_bin = partition_table_bin

    @classmethod
    def name(cls):
        return 'esp32'

    @classmethod
    def capabilities(cls):
        return RunnerCaps(commands={'flash'})

    @classmethod
    def do_add_parser(cls, parser):
        # Required
        parser.add_argument('--esp-idf-path', required=True,
                            help='path to ESP-IDF')
        # Optional
        parser.add_argument('--esp-boot-address', default='0x1000',
                            help='bootloader load address')
        parser.add_argument('--esp-partition-table-address', default='0x8000',
                            help='partition table load address')
        parser.add_argument('--esp-app-address', default='0x10000',
                            help='application load address')
        parser.add_argument('--esp-device', default=os.environ.get('ESPTOOL_PORT', None),
                            help='serial port to flash')
        parser.add_argument('--esp-baud-rate', default='921600',
                            help='serial baud rate, default 921600')
        parser.add_argument('--esp-flash-size', default='detect',
                            help='flash size, default "detect"')
        parser.add_argument('--esp-flash-freq', default='40m',
                            help='flash frequency, default "40m"')
        parser.add_argument('--esp-flash-mode', default='dio',
                            help='flash mode, default "dio"')
        parser.add_argument(
            '--esp-tool',
            help='''if given, complete path to espidf. default is to search for
            it in [ESP_IDF_PATH]/components/esptool_py/esptool/esptool.py''')
        parser.add_argument('--esp-flash-bootloader',
                            help='Bootloader image to flash')
        parser.add_argument('--esp-flash-partition_table',
                            help='Partition table to flash')

    @classmethod
    def do_create(cls, cfg, args):
        if args.esp_tool:
            espidf = args.esp_tool
        else:
            espidf = path.join(args.esp_idf_path, 'components', 'esptool_py',
                               'esptool', 'esptool.py')

        return Esp32BinaryRunner(
            cfg, args.esp_device, boot_address=args.esp_boot_address,
            part_table_address=args.esp_partition_table_address,
            app_address=args.esp_app_address,baud=args.esp_baud_rate,
            flash_size=args.esp_flash_size, flash_freq=args.esp_flash_freq,
            flash_mode=args.esp_flash_mode, espidf=espidf,
            bootloader_bin=args.esp_flash_bootloader,
            partition_table_bin=args.esp_flash_partition_table)

    def do_run(self, command, **kwargs):
        self.require(self.espidf)
        bin_name = path.splitext(self.elf)[0] + path.extsep + 'bin'

        cmd_flash = [self.espidf, '--chip', 'auto']
        if self.device is not None:
            cmd_flash.extend(['--port', self.device])
        cmd_flash.extend(['--baud', self.baud])
        cmd_flash.extend(['--before', 'default_reset'])
        cmd_flash.extend(['--after', 'hard_reset', 'write_flash', '-u'])
        cmd_flash.extend(['--flash_mode', self.flash_mode])
        cmd_flash.extend(['--flash_freq', self.flash_freq])
        cmd_flash.extend(['--flash_size', self.flash_size])

        # Execute Python interpreter if calling a Python script
        if self.espidf.lower().endswith(".py") and sys.executable:
            cmd_flash.insert(0, sys.executable)

        if self.bootloader_bin:
            cmd_flash.extend([self.boot_address, self.bootloader_bin])
            cmd_flash.extend([self.part_table_address, self.partition_table_bin])
            cmd_flash.extend([self.app_address, bin_name])
        else:
            cmd_flash.extend([self.boot_address, bin_name])

        self.logger.info("Flashing esp32 chip on {} ({}bps)".
                         format(self.device, self.baud))
        self.check_call(cmd_flash)
