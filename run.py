import os

def run():
    os.system("west build -b esp32s3_devkitc/esp32s3/procpu app")
    os.system("west flash")
    os.system("west espressif monitor")

if __name__ == "__main__":
    run()