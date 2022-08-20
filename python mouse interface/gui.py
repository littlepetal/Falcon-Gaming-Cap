from PyQt5 import QtGui, QtCore, QtWidgets
import pyqtgraph as pg
import sys


dt = 100


def close_app():
    #timer.stop()
    quit()
    sys.exit()


def doNothing():
    tb1.append("Button pressed!")


def createApp(app):

    if not QtWidgets.QApplication.instance():
        app[0] = QtWidgets.QApplication([])
    else:
        app[0] = QtWidgets.QApplication.instance()

    return app[0]

def initWindow():
    global win, layout, b1, b2, b3, b4, t1, tb1, tb2

    win = QtWidgets.QWidget()
    win.setWindowTitle("Mouse Control App")
    win.resize(500, 500)
    layout = QtWidgets.QGridLayout()
    win.setLayout(layout)

    b1 = QtWidgets.QPushButton("Find Device")
    b1.clicked.connect(doNothing)

    b2 = QtWidgets.QPushButton("Start Stream")
    b2.clicked.connect(doNothing)

    b3 = QtWidgets.QPushButton("Stop Stream")
    b3.clicked.connect(doNothing)

    b4 = QtWidgets.QPushButton("Recentre")
    b4.clicked.connect(doNothing)

    b5 = QtWidgets.QPushButton("Close App")
    b5.clicked.connect(close_app)

    t1 = QtWidgets.QLineEdit("4210")

    tb1 = QtWidgets.QTextBrowser()
    tb2 = QtWidgets.QTextBrowser()

    layout.addWidget(b1, 0, 0)
    layout.addWidget(b2, 0, 2)
    layout.addWidget(b3, 1, 2)
    layout.addWidget(b4, 1, 3)
    layout.addWidget(b5, 0, 3)

    layout.addWidget(t1, 0, 1, 1, 1)

    layout.addWidget(tb1,1, 0, 2, 2)
    layout.addWidget(tb2,2, 2, 1, 2)



def update():
    app.processEvents()


def startGui():
    global timer
    timer = QtCore.QTimer()
    timer.timeout.connect(update)
    timer.start(dt)
    timer.setInterval(dt)
    win.show()
    app.exec_()


def uiThread(appContainer):
    global app
    app = createApp(appContainer)
    win = initWindow()
    startGui()



#try:
#    while True:
#        pass
#except KeyboardInterrupt:
#    close_app()
