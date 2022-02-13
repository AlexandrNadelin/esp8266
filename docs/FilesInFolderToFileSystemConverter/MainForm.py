from ast import Not
from PyQt5 import QtGui, uic
from PyQt5.QtWidgets import (QTextEdit, QWidget, QVBoxLayout, QHBoxLayout,QLabel, QApplication,QLineEdit,QFrame,QPushButton,QFileDialog,QGridLayout,QDesktopWidget)
from PyQt5.QtGui import QFont
import sys  # We need sys so that we can pass argv to QApplication
import os
import os.path
import FileSystemCreator

class MainForm(QWidget):    

    def selectFolder(self):
        fileWriteFileSystemPath = ""
        folderWithFilesPath = ""        
        if(os.path.exists('properties')):            
            fileProperties = open('properties','r')
            folderWithFilesPath = fileProperties.readline()
            fileWriteFileSystemPath = fileProperties.readline()
            fileProperties.close()
        else:
            fileWriteFileSystemPath = os.path.abspath(os.curdir)
            folderWithFilesPath = os.path.abspath(os.curdir)
            fileProperties = open('properties','w+')
            fileProperties.write(fileWriteFileSystemPath)
            fileProperties.write(folderWithFilesPath)
            fileProperties.close()

        destFile = QFileDialog.getSaveFileName(self,"Open write file system destination file", fileWriteFileSystemPath, "Files (*.c *.h)")
        if(destFile[0]==''):
            return
        folderPath = QFileDialog.getExistingDirectory(self, "Select Directory",folderWithFilesPath)
        if(folderPath==''):
            return
        FileSystemCreator.createFileSystem(folderPath,destFile[0])

    def __init__(self):
        super().__init__()

        self.setFont(QFont('Arial', 12))

        mainVLayout = QVBoxLayout()
        self.selectFolderBTN = QPushButton("Select files")
        self.selectFolderBTN.setMinimumSize(170,40)
        self.selectFolderBTN.clicked.connect(self.selectFolder)
        mainVLayout.addWidget(self.selectFolderBTN)

        self.setLayout(mainVLayout)

        self.setGeometry(0,0,400,200)
        self.setMinimumHeight(200)
        self.setWindowTitle('Vibo monitoring app')

        qtRectangle = self.frameGeometry()
        centerPoint = QDesktopWidget().availableGeometry().center()
        qtRectangle.moveCenter(centerPoint)
        self.move(qtRectangle.topLeft())

        self.show()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = MainForm()
    sys.exit(app.exec_())