import json
data1 = "{'type': 'STM_SENSOR', 'T': 30.05, 'H': 100.0, 'P': 1010.75, 'Mag': {'X': 403, 'Y': -153, 'Z': -669}, 'Gyr': {'X': -210.0, 'Y': -840.0, 'Z': 910.0}, 'Acc': {'X': 36, 'Y': -74, 'Z': 1001}, 'dest': '2222', 'time': 1653153151.100985}"
data2 = '{"type": "STM_SENSOR"}'
# print(data2)
print(json.loads(data2))