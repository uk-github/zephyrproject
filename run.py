import os
import argparse

def parse_args():
    parser = argparse.ArgumentParser(description="Build, flash, and monitor Zephyr app.")
    parser.add_argument("--board", default="esp32s3_devkitc/esp32s3/procpu", help="Target board")
    parser.add_argument("--app", default="app", help="Application directory")
    #parser.add_argument("--ver", default="zephyr", help="Application directory")
    parser.add_argument("--pristine", action="store_true", help="Use pristine build")
    parser.add_argument("--flash", action="store_true", help="Use flash to run in board")
    parser.add_argument("--serial", action="store_true", help="Use serial to open espressif monitor")
    return parser.parse_args()

def run():
    args = parse_args()
    build_cmd = f"west build -b {args.board} {args.app}"
    if args.pristine:
        build_cmd += " --pristine"
    
    if os.system(build_cmd) == 0:
        print("\033[91mBuild success \033[0m")
    else:
        print("\033[91mBuild failed\033[0m")
        exit(1)
    
    if args.flash:
        os.system("west flash")

    if args.flash and args.serial:
        os.system("west espressif monitor")
    elif args.serial:
        print("\033[91mflash first\033[0m")


if __name__ == "__main__":
        run()