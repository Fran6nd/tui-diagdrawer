import tkinter
from menuBar import MenuBar

class Application:
    def __init__(self):
        # Create and display a test window for viewing the menus
        window = tkinter.Tk()
        window.minsize(500, 500)

        # Create an instance of the MenuBar class and display it to the window
        mb = MenuBar(window)
        # window.config(menu = mb)

        window.mainloop()

if __name__ == '__main__':
    Application()