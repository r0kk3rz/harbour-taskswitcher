import dbus
from xml.etree import ElementTree
import subprocess

def introspect(path):
    system_bus = dbus.SystemBus()
    proxy = system_bus.get_object('org.bluez',path)
    iface = dbus.Interface(proxy, 'org.freedesktop.DBus.Introspectable')

    nodes = []
    xml_string = iface.Introspect()
    for child in ElementTree.fromstring(xml_string):
        if child.tag == 'node':
            nodes.append('/'.join((path, child.attrib['name'])))

    return nodes

def list_interfaces():
    return introspect('/org/bluez')

def list_devices(interfaces):
    devices = []

    for iface in interfaces:
        device_list = introspect(iface)
        for device in device_list:
            devices.append(device)
    return devices


def device_name(path):
    system_bus = dbus.SystemBus()
    proxy = system_bus.get_object('org.bluez',path)

    iface = dbus.Interface(proxy, 'org.freedesktop.DBus.Properties')

    return iface.GetAll('org.bluez.Device1')['Name']


interfaces = list_interfaces()
devices = list_devices(interfaces)
deviceNames = []
count = 0
for dev in devices:
    name = device_name(dev)
    print(str(count) + ': ' + name)
    deviceNames.append(name)
    count += 1

choice = input('Choose Device: ')

devName = deviceNames[int(choice)]

subprocess.call(['dconf', 'write', '/uk/co/piggz/taskswitcher/deviceName', "'"+ devName +"'"])

subprocess.call(['dconf', 'read', '/uk/co/piggz/taskswitcher/deviceName'])