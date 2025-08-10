import os
import argparse

def parse_args():
    parser = argparse.ArgumentParser(description="Build, flash, and monitor Zephyr app.")
    parser.add_argument("--board", default="esp32s3_devkitc/esp32s3/procpu", help="Target board")
    parser.add_argument("--app", default="app", help="Application directory")
    parser.add_argument("--pristine", action="store_true", help="Use pristine build")
    return parser.parse_args()

args = parse_args()

def run():
    build_cmd = f"west build -b {args.board} {args.app}"
    if args.pristine:
        build_cmd += " --pristine"
    
    if os.system(build_cmd) == 0:
        print("\033[91mBuild success \033[0m")
        os.system("west flash")
        os.system("west espressif monitor")
    else:
        print("\033[91mBuild failed\033[0m")
        exit(1)

if __name__ == "__main__":
        run()