import tkinter


class MenuBar:
    def __init__(self, window):
        # Create and display the main menu bar
        menuBar = tkinter.Menu(window)

        # Create a pull-down menu for file operations
        fileMenu = tkinter.Menu(menuBar, tearoff=False)
        fileMenu.add_command(label="New")
        fileMenu.add_command(label="Open...")
        fileMenu.add_command(label="Close")
        fileMenu.add_command(label="Exit")
        menuBar.add_cascade(menu=fileMenu, label="File")

        # Create a pull-down menu for editing operations
        editMenu = tkinter.Menu(menuBar, tearoff=False)
        editMenu.add_command(label="Cut")
        editMenu.add_command(label="Copy")
        editMenu.add_command(label="Paste")
        editMenu.add_command(label="Select All")
        editMenu.add_command(label="Undo")
        editMenu.add_command(label="Redo")
        menuBar.add_cascade(menu=editMenu, label="Edit")
