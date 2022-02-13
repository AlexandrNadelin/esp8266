import sys  # We need sys so that we can pass argv to QApplication
from MainForm import MainForm
from PyQt5.QtWidgets import (QApplication)
import FileSystemCreator

if sys.argv.__len__()==3:
    FileSystemCreator.createFileSystem(sys.argv[1],sys.argv[2])
else:    
    app = QApplication(sys.argv)
    w = MainForm()
    sys.exit(app.exec_())

