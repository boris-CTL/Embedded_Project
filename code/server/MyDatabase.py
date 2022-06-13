import os
from pymongo import MongoClient
import json

def get_database():
    DB_PASSWORD = os.getenv("DB_PASSWORD")
    DB_NAME = os.getenv("DB_NAME")
    # Provide the mongodb atlas url to connect python to mongodb using pymongo
    CONNECTION_STRING = "mongodb+srv://Sam0403:"+DB_PASSWORD+"@sensordata.nlixt.mongodb.net/"+DB_NAME+"?retryWrites=true&w=majority"

    # Create a connection using MongoClient. You can import MongoClient or use pymongo.MongoClient
    client = MongoClient(CONNECTION_STRING)

    # Create the database for our example (we will use the same database throughout the tutorial
    return client[DB_NAME]

# data = '{"type":"STM32", "T":30.95, "H":85.49, "P":1006.30, "Mag":{"Mag_X":-3622, "Mag_Y":762, "Mag_Z":2217}, "Gyr":{"Gyr_X":910.00, "Gyr_Y":-3640.00, "Gyr_Z":630.00}, "Acc":{"Acc_X":96, "Acc_Y":0, "Acc_Z":994}}'
# data_example = {
#     "type":"STM32",
#     "T":30.95,
#     "H":85.49,
#     "P":1006.30,
#     "Mag":{"Mag_X":-3622, "Mag_Y":762, "Mag_Z":2217},
#     "Gyr":{"Gyr_X":910.00, "Gyr_Y":-3640.00, "Gyr_Z":630.00},
#     "Acc":{"Acc_X":96, "Acc_Y":0, "Acc_Z":994}
# }
