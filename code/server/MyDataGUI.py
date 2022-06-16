import json
import os
from dotenv import load_dotenv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Button
from collections import deque
import time

from MyDatabase import get_database

class MyDataGUI:
    def __init__(self):
        # self.plotting = False
        self.acc_X = deque()
        self.acc_Y = deque()
        self.acc_Z = deque()
        self.gyr_X = deque()
        self.gyr_Y = deque()
        self.gyr_Z = deque()
        self.mag_X = deque()
        self.mag_Y = deque()
        self.mag_Z = deque()
        self.time = deque()
        self.num = 0

        self.mapping_of_title = dict()
        self.mapping_of_title[(0,0)] = 'Acc-x'
        self.mapping_of_title[(0,1)] = 'Acc-y'
        self.mapping_of_title[(0,2)] = 'Acc-z'
        self.mapping_of_title[(1,0)] = 'Gyr-x'
        self.mapping_of_title[(1,1)] = 'Gyr-y'
        self.mapping_of_title[(1,2)] = 'Gyr-z'
        
        self.mapping_of_those_list_of_values = dict()
        self.mapping_of_those_list_of_values[(0,0)] = self.acc_X
        self.mapping_of_those_list_of_values[(0,1)] = self.acc_Y
        self.mapping_of_those_list_of_values[(0,2)] = self.acc_Z
        self.mapping_of_those_list_of_values[(1,0)] = self.gyr_X
        self.mapping_of_those_list_of_values[(1,1)] = self.gyr_Y
        self.mapping_of_those_list_of_values[(1,2)] = self.gyr_Z
        
        self.mapping_of_colors = dict()
        self.mapping_of_colors[(0,0)] = 'b'
        self.mapping_of_colors[(0,1)] = 'g'
        self.mapping_of_colors[(0,2)] = 'r'
        self.mapping_of_colors[(1,0)] = 'c'
        self.mapping_of_colors[(1,1)] = 'y'
        self.mapping_of_colors[(1,2)] = 'k'

        # self.fig, self.axs = plt.subplots(2, 3)
        # plt.tight_layout()

    def setData(self, latestData):
        self.num = 0
        self.time.clear()
        self.acc_X.clear()
        self.acc_Y.clear()
        self.acc_Z.clear()
        self.gyr_X.clear()
        self.gyr_Y.clear()
        self.gyr_Z.clear()
        for data in latestData:
            self.time.append(time.ctime(data["time"]))
            self.acc_X.append(data["Acc"]["X"])
            self.acc_Y.append(data["Acc"]["Y"])
            self.acc_Z.append(data["Acc"]["Z"])
            self.gyr_X.append(data["Gyr"]["X"])
            self.gyr_Y.append(data["Gyr"]["Y"])
            self.gyr_Z.append(data["Gyr"]["Z"])
            self.num += 1

    def __plot__(self, i):
        self.mapping_of_those_list_of_values[(0,0)] = self.acc_X
        self.mapping_of_those_list_of_values[(0,1)] = self.acc_Y
        self.mapping_of_those_list_of_values[(0,2)] = self.acc_Z
        self.mapping_of_those_list_of_values[(1,0)] = self.gyr_X
        self.mapping_of_those_list_of_values[(1,1)] = self.gyr_Y
        self.mapping_of_those_list_of_values[(1,2)] = self.gyr_Z

        for index in range(0,2):
            for j in range(0,3):
                self.axs[index,j].cla()
                self.axs[index,j].set_xticks([])
                self.axs[index,j].set_title(self.mapping_of_title[(index,j)])
                self.axs[index,j].grid(True, linewidth=0.5, linestyle=':')
        
        for index in range(0,2):
            for j in range(0,3):
                self.axs[index,j].plot(self.time, self.mapping_of_those_list_of_values[(index,j)], self.mapping_of_colors[(index,j)])
        # plt.pause(0.1)
        # try:
        #     plt.pause(0.1)
        #     # self.fig.pause(0.1)
        # except:
        #     # print("Exception Close")
        #     pass

    def updateData(self, data):
        if self.num >= 50:
            self.time.popleft()
            self.acc_X.popleft()
            self.acc_Y.popleft()
            self.acc_Z.popleft()
            self.gyr_X.popleft()
            self.gyr_Y.popleft()
            self.gyr_Z.popleft()
        else:
            self.num += 1
        self.acc_X.append(data["Acc"]["X"])
        self.acc_Y.append(data["Acc"]["Y"])
        self.acc_Z.append(data["Acc"]["Z"])
        self.gyr_X.append(data["Gyr"]["X"])
        self.gyr_Y.append(data["Gyr"]["Y"])
        self.gyr_Z.append(data["Gyr"]["Z"])
        self.time.append(data["time"])

    def __close_event__(self, event):
        try:
            plt.close(self.fig)
        except:
            pass

    def start(self):
        # start_new_thread(self.control, ())
        self.fig, self.axs = plt.subplots(2, 3)
        self.fig.canvas.mpl_connect('close_event', self.__close_event__)
        # self.close_ax = self.fig.add_axes([0.025,0.025,0.125,0.05])
        # self.close_but = Button(self.close_ax,'Close Plot',color='#FCFCFC',hovercolor='w')
        # self.close_but.on_clicked(self.__close_event__)
        plt.tight_layout()
        # self.fig.tight_layout()
        ani = animation.FuncAnimation(self.fig, self.__plot__, interval=100)
        plt.show()

if __name__ == '__main__':
    load_dotenv()
    DB_COLLECTION_NAME = os.getenv("DB_COLLECTION_NAME")
    # Get the database
    DBName = get_database()
    collection = DBName[DB_COLLECTION_NAME]

    data = collection.find().sort("_id", 1).limit(50)

    newData = []

    for item in data:
        newData.append(str(item))

    DBdata = {
        "data": str(newData)
    }
    for item in json.loads(DBdata["data"]):
        print(item)

    # print(list(data))

    # myDataGUI = MyDataGUI(collection)
    # myDataGUI.getData()
    # myDataGUI.start()