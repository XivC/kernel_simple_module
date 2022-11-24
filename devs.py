import glob

def find_pcis():
    base_dir = '/sys/bus/pci/devices/*'

    file_names = glob.glob(base_dir)

    print('PCI_SLOT_NAME' + ' ' * 10 + 'VENDOR_ID(0x10)' + ' ' * 10 + 'DEVICE_ID(0x10)' + ' ' * 10)
    for file_name in file_names:
        with open(file_name + '/uevent') as f:
            data = f.readlines()
            pci_slot_name = ''
            vendor_id = ''
            device_id = ''

            for line in data:
                if 'PCI_SLOT_NAME' in line:
                    pci_slot_name = line.split('=')[1].replace('\n', '')
                elif 'PCI_ID' in line:
                    vendor_id, device_id = line.split('=')[1].split(':')
                    vendor_id, device_id = int(vendor_id, 16), int(device_id, 16)

            print(pci_slot_name + ' ' * 14  + str(vendor_id) + ' ' * (18 - len(str(vendor_id)) + 4) + str(device_id))

find_pcis()
