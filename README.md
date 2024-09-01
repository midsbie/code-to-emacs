# CodeToEmacs

CodeToEmacs is a tool designed to bridge the gap between the Unity Editor and Emacs, letting
you edit your C# scripts in your favorite editor.

## Problem Statement

When working with Unity, setting Visual Studio Code as the external script editor ensures that Unity
generates the required `csproj` files, which are necessary for running an LSP server. However, if
you prefer using Emacs, setting it as the external editor in Unity's preferences won't generate
these project files, leading to issues with LSP support for C# scripts.

## Solution

CodeToEmacs replaces the `code` binary on your Ubuntu system with a small program that opens C#
scripts in Emacs. This allows Unity to think it's interacting with Visual Studio Code, thereby
continuing to generate the necessary `csproj` files while seamlessly integrating Emacs into your
workflow.

## Getting Started

### Installation

1. Clone the repository:

        git clone git@github.com:midsbie/code-to-emacs.git

1. Configure the paths:

   Since the current implementation does not automatically discover binaries, open [code.c](code.c)
   and ensure the paths in the macros `PATH_EMACSCLIENT`, `PATH_SH`, and `PATH_CODE` correctly point
   to the respective binaries.

1. Ensure Visual Studio Code is installed on your system:

   Unity performs auto-detection to verify the presence of Visual Studio Code.

   1. If the Unity editor is running and you've just installed Visual Studio Code, restart Unity.

1. Build the project:

        cd code-to-emacs
        make

1. Install the binary:

   1. Replace VSCode binary with the one built above. On a typical Ubuntu system, this would involve
      the steps:
      - `sudo mv /usr/share/code/code /usr/share/code/code.ORIG`
      - `sudo cp /path/to/code-to-emacs/code /usr/share/code/`

### Usage

If everything is set up correctly, attempting to edit a component in Unity should now open it in
Emacs.

## Contributing

Contributions are what make the open-source community such an amazing place to learn, inspire, and
create. All contributions are greatly appreciated.

## License

Distributed under the MIT License. See LICENSE for more information.
