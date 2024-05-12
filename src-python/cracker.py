import os
import random
import subprocess
import time
import platform
import shutil

def generate_random_pass(digit_count):
    min_value = 10 ** (digit_count - 1)
    max_value = (10 ** digit_count) - 1
    return str(random.randint(min_value, max_value))

def get_user_digit_count():
    while True:
        try:
            digit_count = int(input("How many digits should the random number have? "))
            if digit_count > 0:
                return digit_count
            else:
                print("Please enter a positive number.")
        except ValueError:
            print("Please enter a valid number.")


def run_7z(password, path_to_file):
    if not shutil.which("7z") and not shutil.which("7za"):
        print("7z command not found. Please make sure 7z is installed.")
        return

    if platform.system() == "Windows":
        command = f"7z x -p{password} {path_to_file}"
    elif platform.system() == "Linux":
        if shutil.which("7za"):
            command = f"7za x -p{password} {path_to_file}"
        elif shutil.which("xz"):
            command = f"xz -d -c -k -T0 -q -e -p{password} {path_to_file} > {os.path.splitext(path_to_file)[0]}"
        else:
            command = f"7z x -p{password} {path_to_file}"
    else:
        print("Your system is not supported. This tool only works on Windows and Linux.")
        return
    try:
        process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
        output, error = process.communicate(input=b"n\n")  # Provide "n" as input to skip extracting the file
        return process.returncode
    except subprocess.CalledProcessError:
        print("7z command not found in PATH. Please make sure 7z is installed.")
        return


def save_progress(failed_passes, path_to_file):
    if not os.path.exists("data"):
        os.makedirs("data")

    progress_file = fr"data/{os.path.splitext(path_to_file)[0]}.txt"
    with open(progress_file, 'w') as file:
        file.write('\n'.join(failed_passes))


def crack_7z_file():
    print(r" _____ _________  ___ ____  ")
    print(r"|___  |__  /  _ \|_ _|  _ \ ")
    print(r"   / /  / /| |_) || || |_) |")
    print(r"  / /  / /_|  _ < | ||  __/ ")
    print(r" /_/  /____|_| \_\___|_|   ")
    print(r"-------------------------")
    print(r'| Written By WaltuhTabs |')
    print(r'-------------------------')
    print(r"7ZRIP is a brute-force tool that cracks rar, 7z, and zip passwords")
    print('NOTE: This tool was made for educational purposes only. Use at your own risk.')
    print('To avoid progress corrupt , use CTRL + C')
    print('')

    failed_passes = set()
    path_to_file = input("What is the path to the 7z file? ")
    while not os.path.exists(path_to_file):
        print("Invalid file path. Please enter a valid path.")
        path_to_file = input("What is the path to the 7z file? ")

    method = input("Generate brute force (Only numbers) (g) or get from a file (f)? ")
    while method.lower() not in ['g', 'f']:
        print("Invalid method. Please choose 'g' for generate or 'f' for file.")
        method = input("Generate brute force (Only numbers) (g) or get from a file (f)? ")

    log_false_pass = input("Do you want false passwords to be written (Y/N)? ")
    while log_false_pass.lower() not in ['y', 'n', 'yes', 'no']:
        print("Invalid. Please choose 'y' to agree or 'n' to disagree")
        log_false_pass = input("Do you want false passwords to be written (Y/N)? ")

    if method.lower() == 'g':
        digit_count = get_user_digit_count()
    elif method.lower() == 'f':
        txtfile = input("What is the password file? ")
        while not os.path.exists(txtfile):
            print("Invalid file path. Please enter a valid path.")
            txtfile = input("What is the password file? ")

        with open(txtfile, 'r', encoding='latin-1') as file:
            passwords = [line.strip() for line in file]
            if len(passwords) == 0:
                print("Password file is empty. Please provide a valid password file.")
                return

    print('Cracking Started')

    progress_file = fr"data\{os.path.splitext(path_to_file)[0]}.txt"
    if os.path.exists(progress_file):
        resume = input("A progress file was found. Do you want to resume cracking from where you left off? (Y/N) ")
        while resume.lower() not in ['y', 'n', 'yes', 'no']:
            print("Invalid input. Please choose 'Y' to resume or 'N' to start from the beginning.")
            resume = input("Do you want to resume cracking from where you left off? (Y/N) ")

        if resume.lower() in ['y', 'yes']:
            with open(progress_file, 'r') as file:
                failed_passes = set(file.read().splitlines())
        else:
            os.remove(progress_file)

    try:
        while True:
            if method.lower() == 'g':
                password = generate_random_pass(digit_count)
            elif method.lower() == 'f':
                password = random.choice(passwords)

            if password in failed_passes:
                continue

            result = run_7z(password, path_to_file)
            if result == 0:
                print(f"Password FOUND: {password} | After {len(failed_passes)} tries...")
                with open(f'found_pass_{os.path.basename(path_to_file)}.txt', 'w') as file:
                    file.write(password)
                break
            else:
                failed_passes.add(password)
                if log_false_pass.lower() in ['y', 'yes']:
                    print(f"Password FAILED | TRY NUM#{len(failed_passes)} | {password}")

            save_progress(failed_passes, path_to_file)

    except KeyboardInterrupt:
        print("Cracking process interrupted. Saving progress...")
        save_progress(failed_passes, path_to_file)

crack_7z_file()
