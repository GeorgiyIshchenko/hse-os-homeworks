import re
import os
import subprocess


HW_DIR_RE = re.compile(r"^hw_[0-9]+$")
RUN_FILE_NAME = "run.sh"

old_dir = os.path.abspath(os.path.dirname(__file__))

for name in os.listdir("."):
    if HW_DIR_RE.search(name) != None:
        print(f"RUNNING HOMEWORK: {name}")
        os.chdir(name)
        run_path = os.path.join(os.path.abspath(
            os.path.dirname(name)), RUN_FILE_NAME)
        subprocess.run(run_path, shell=True)
        os.chdir(old_dir)
        print()
