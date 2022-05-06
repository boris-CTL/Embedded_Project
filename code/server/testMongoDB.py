from pymongo import MongoClient
from dotenv import load_dotenv
import os

def get_database():
    DB_PASSWORD = os.getenv("DB_PASSWORD")
    DB_NAME = os.getenv("DB_NAME")
    # Provide the mongodb atlas url to connect python to mongodb using pymongo
    CONNECTION_STRING = "mongodb+srv://Sam0403:"+DB_PASSWORD+"@sensordata.nlixt.mongodb.net/"+DB_NAME+"?retryWrites=true&w=majority"

    # Create a connection using MongoClient. You can import MongoClient or use pymongo.MongoClient
    client = MongoClient(CONNECTION_STRING)

    # Create the database for our example (we will use the same database throughout the tutorial
    return client[DB_NAME]
    
# This is added so that many files can reuse the function get_database()
if __name__ == "__main__":    
    load_dotenv()
    DB_COLLECTION_NAME = os.getenv("DB_COLLECTION_NAME")
    # Get the database
    DBName = get_database()

    collectionName = DBName[DB_COLLECTION_NAME]

    # Insert Sample Data
    item_1 = {
        "item_name" : "Blender",
        "max_discount" : "10%",
        "batch_number" : "RR450020FRG",
        "price" : 340,
        "category" : "kitchen appliance"
    }
    item_2 = {
        "item_name" : "Egg",
        "category" : "food",
        "quantity" : 12,
        "price" : 36,
        "item_description" : "brown country eggs"
    }
    collectionName.insert_many([item_1,item_2])

    # Find Sample Data
    items1 = collectionName.find()
    for item in items1:
        print(item)
    items2 = collectionName.find({"category" : "food"})
    for item in items1:
        print(item)