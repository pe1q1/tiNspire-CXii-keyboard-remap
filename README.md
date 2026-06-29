# TI-Nspire CX II Custom Keyboard Remap

<p align="center">
  <img alt="Device: TI-Nspire CX II CAS" src="https://img.shields.io/badge/device-TI--Nspire%20CX%20II%20CAS-2f6f9f">
  <img alt="OS: 6.2.0.333" src="https://img.shields.io/badge/OS-6.2.0.333-3b7f3b">
  <img alt="Runtime: Ndless" src="https://img.shields.io/badge/runtime-Ndless-c46d1f">
  <img alt="Language: C" src="https://img.shields.io/badge/language-C-555555">
</p>

This project adds a QWERTY-style alphabet remap for the TI-Nspire CX II CAS.
It is built as a small Ndless app that rewrites alphabetic key events before
the OS delivers them to the active input field.

<p align="center">
  <img src="assets/fast-calculator.gif" width="900" alt="Fast calculator demo">
</p>

## The Problem

The stock alpha layout is alphabetical by key position, which makes typing
commands, variable names, and longer text slower if you are used to a normal
keyboard. The goal was to keep the physical calculator keys as-is while making
the output feel closer to QWERTY.

## The Fix

The app installs a resident hook on the OS event queue for TI-Nspire CX II CAS
OS `6.2.0.333`. The hook is loaded into RAM by Ndless and attached to the
`send_to_event_queue` function, which is the low-level path the OS uses before
key events reach the active text field, console, or document input.

At that level, each key press is passed as an `s_ns_event` structure. The hook
reads the event pointer from the saved CPU register state, checks that the
event is an alphabetic key event, then rewrites the ASCII byte in the event
structure before control returns to the OS. The physical key code still comes
from the calculator keyboard, but the character delivered upward is changed to
the QWERTY-style output letter.

The remap keeps the edit small and predictable:

- only A-Z/a-z ASCII events are rewritten
- modifier combinations are left alone
- non-letter keys pass through unchanged
- the event's high ASCII bits are preserved
- the low byte of the key field is updated when it matches the original letter

The install path checks the OS ID, OS signature, and first two ARM instruction
words at `send_to_event_queue` before installing the RAM hook, so the remap is
attached to the expected CX II CAS `6.2.0.333` event-queue layout.

The program also includes a probe mode that prints `source -> mapped` rows, so
the mapping can be checked directly on the calculator screen before installing
the remap.

<p align="center">
  <img src="assets/terminal-calc.png" width="700" alt="Terminal calculator output">
</p>

## Layout

```text
Physical alpha layout        Custom output layout
A B C D E F G       ->      Q W E R T Y U
H I J K L M N       ->      A S D F G H J
O P Q R S T U       ->      K L Z X C V B
V W X Y Z           ->      N M P O I
```

## How It Works

- `qwerty_map_char()` contains the A-Z remap table.
- `rewrite_key_event()` edits alphabetic OS key events in place.
- `HOOK_INSTALL()` attaches the remap to `send_to_event_queue`.
- The install path checks the OS ID, OS signature, and function fingerprint for
  the CX II CAS OS `6.2.0.333` target before installing.

## Install On Calculator

For normal use, the calculator only needs to be jailbroken with Ndless.

1. Copy `qwerty_keymap.tns` onto the calculator.
2. Open and run `qwerty_keymap.tns`.
3. Choose `1` to probe the key mapping or `2` to install the QWERTY remap.

If you already have the `.tns` file, there is no extra setup step on the
calculator side.

## Build

Target calculator:

- TI-Nspire CX II CAS
- OS `6.2.0.333`
- Ndless installed

Build host:

- Linux, macOS, or Windows through WSL/MSYS2/Git Bash
- a shell that can run `make` and the Ndless command-line tools

Install the Ndless SDK and make sure these commands are on `PATH`:

```text
nspire-gcc
nspire-ld
genzehn
make-prg
make
```

Then build:

```sh
cd src
make clean all
```

The output is created at:

```text
src/build/qwerty_keymap.tns
```

That `.tns` file is the file to copy onto the calculator and run.

## Files

- `src/qwerty_keymap.c`: remap logic and install menu.
- `src/Makefile`: Ndless build.
- `assets/fast-calculator.gif`: calculator demo.
- `assets/terminal-calc.png`: terminal screenshot.
