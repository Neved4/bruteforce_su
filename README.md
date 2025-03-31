[![C](https://img.shields.io/badge/ISO_C17-A8B9CC?logo=c&logoColor=fff)][C17]
[![IEEE](https://img.shields.io/badge/POSIX.1&#8209;2024-00629B?logo=ieee&logoColor=fff)][POSIX.1-2024]

# bruteforce_su

[`bruteforce_su`] is a tool that attempts to brute-force user authentication via `su` using a provided password dictionary. It automates password guessing by simulating user input and monitoring process behavior.

> [!CAUTION]
> This tool is strictly for educational and research purposes.
> Unauthorized use of brute-force tools is illegal and unethical.

## Features

- **Automated brute-force**: Tries passwords from a dictionary file.
- **Process monitoring**: Ensures execution control through timeout.
- **Safe execution**: Handles errors in process management and I/O.
- **Customizable timeout**: Allows specifying a timeout for login attempts.

## Getting Started

### Prerequisites

To build [`bruteforce_su`], you need:

- A C compiler that supports [C17] or later.
- A POSIX-compliant system (Linux, macOS, BSD).
- Root privileges for `su` access testing.

### Installing

Clone the repository and compile the program:

```sh
$ git clone https://github.com/your-repo/bruteforce_su.git
$ cd bruteforce_su
$ make
```

[`arm64`]: https://en.wikipedia.org/wiki/AArch64
[`bruteforce_su`]: https://github.com/Neved4/bruteforce_su
[`clang`]: https://clang.llvm.org/
[`gcc`]: https://gcc.gnu.org/
[`tcc`]: https://bellard.org/tcc/
[`x86_64`]: https://en.wikipedia.org/wiki/X86-64
[`zig`]: https://ziglang.org/
[C99]: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf
[C17]: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2310.pdf
[C23]: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3220.pdf
[GPLv3 License]: https://www.gnu.org/licenses/gpl-3.0.en.html
[Homebrew]: https://brew.sh/
[POSIX.1-2024]: https://pubs.opengroup.org/onlinepubs/9799919799/

[^1]: _IEEE Std 1003.1-2024: Standard for Information Technology
    — Portable Operating System Interface (POSIX®)_, \
    ISO/IEC DIS 9945. URL: https://pubs.opengroup.org/onlinepubs/9799919799/
[^2]: _ISO/IEC 9899: Standard for Information Technology
    — Programming languages — C_, ISO/IEC 9899:2023. \
    URL: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf
