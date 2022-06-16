import numpy as np
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from matplotlib.widgets import Button

class MyRadarGUI:
    def __init__(self):
        self.angle = 0
        self.dist = 0
        self.stop_bool = False
        self.close_bool = False
    def __stop_event__(self, event):
        self.stop_bool = 1
    def __close_event__(self, event):
        if self.stop_bool:
            # plt.close('all')
            try:
                plt.close(self.fig)
            except:
                pass
        self.stop_bool = 1
        self.close_bool = 1
    def start(self):
        self.stop_bool = False
        self.close_bool = False
        self.fig = plt.figure(facecolor='k')
        self.win = self.fig.canvas.manager.window # figure window
        self.screen_res = self.win.wm_maxsize() # used for window formatting later
        self.dpi = 150.0 # figure resolution
        self.fig.set_dpi(self.dpi) # set figure resolution

        # polar plot attributes and initial conditions
        self.ax = self.fig.add_subplot(111,polar=True,facecolor='#006d70')
        self.ax.set_position([-0.05,-0.05,1.1,1.05])
        self.r_max = 500.0 # can change this based on range of sensor
        self.ax.set_ylim([0.0,self.r_max]) # range of distances to show
        self.ax.set_xlim([0.0,np.pi]) # limited by the servo span (0-180 deg)
        self.ax.tick_params(axis='both',colors='w')
        self.ax.grid(color='w',alpha=0.5) # grid color
        self.ax.set_rticks(np.linspace(0.0,self.r_max,5)) # show 5 different distances
        self.ax.set_thetagrids(np.linspace(0.0,180.0,10)) # show 10 angles
        self.angles = np.arange(0,181,1) # 0 - 180 degrees
        self.theta = self.angles*(np.pi/180.0) # to radians
        self.dists = np.ones((len(self.angles),)) # dummy distances until real data comes in
        self.pols, = self.ax.plot([],linestyle='',marker='o',markerfacecolor = 'w',
                        markeredgecolor='#EFEFEF',markeredgewidth=1.0,
                        markersize=5.0,alpha=0.9) # dots for radar points
        self.line1, = self.ax.plot([],color='w',linewidth=4.0) # sweeping arm plot
        self.fig.set_size_inches(0.96*(self.screen_res[0]/self.dpi),0.96*(self.screen_res[1]/self.dpi))
        self.plot_res = self.fig.get_window_extent().bounds # window extent for centering
        self.win.wm_geometry('+{0:1.0f}+{1:1.0f}'.\
                        format((self.screen_res[0]/2.0)-(self.plot_res[2]/2.0),
                            (self.screen_res[1]/2.0)-(self.plot_res[3]/2.0))) # centering plot
        self.fig.canvas.toolbar.pack_forget() # remove toolbar for clean presentation
        self.fig.canvas.set_window_title('Arduino Radar')

        self.fig.canvas.draw() # draw before loop
        self.axbackground = self.fig.canvas.copy_from_bbox(self.ax.bbox) # background to keep during loop

        self.prog_stop_ax = self.fig.add_axes([0.85,0.025,0.125,0.05])
        self.pstop = Button(self.prog_stop_ax,'Stop Program',color='#FCFCFC',hovercolor='w')
        self.pstop.on_clicked(self.__stop_event__)

        self.close_ax = self.fig.add_axes([0.025,0.025,0.125,0.05])
        self.close_but = Button(self.close_ax,'Close Plot',color='#FCFCFC',hovercolor='w')
        self.close_but.on_clicked(self.__close_event__)
        self.fig.show()
        while True:
            try:
                if self.stop_bool: # stops program
                    self.fig.canvas.toolbar.pack_configure() # show toolbar
                    if self.close_bool: # closes radar window
                    #     plt.close('all')
                        plt.close(self.fig)
                    break

                # angle,dist = vals # separate into angle and distance
                if self.dist>self.r_max:
                    self.dist = 0.0 # measuring more than r_max, it's likely inaccurate
                self.dists[int(self.angle)] = self.dist
                # if self.angle % 5 ==0: # update every 5 degrees
                self.pols.set_data(self.theta,self.dists)
                self.fig.canvas.restore_region(self.axbackground)
                self.ax.draw_artist(self.pols)

                self.line1.set_data(np.repeat((self.angle*(np.pi/180.0)),2),
                np.linspace(0.0,self.r_max,2))
                self.ax.draw_artist(self.line1)

                self.fig.canvas.blit(self.ax.bbox) # replot only data
                self.fig.canvas.flush_events() # flush for next plot
            except KeyboardInterrupt:
                # plt.close('all')
                plt.close(self.fig)
                break
    def updateData(self, angle, dist):
        self.angle = angle
        self.dist = dist

if __name__ == "__main__":
    myRadarGUI = MyRadarGUI()
    myRadarGUI.start()
