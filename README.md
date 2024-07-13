# VSCode Wrapper For Emacs

Unity by default integrates with VSCode for editing scripts and other code files on Ubuntu. The
VSCode Wrapper for Emacs is a tool designed to redirect Unity's requests to open files in VSCode to
instead open them in Emacs. This wrapper intercepts the command issued by Unity to VSCode and
transforms it so that the file is opened using Emacs' `emacsclient`. It provides a seamless
integration for users who prefer using Emacs for their development work while working within the
Unity environment.

## Getting Started

### Installation

1. Clone the repository:

        git clone git@github.com:midsbie/vscode-emacs-wrapper.git

1. Since the current implementation does not automatically discover binaries, open [code.c](code.c)
   and ensure the paths in the macros `PATH_EMACSCLIENT`, `PATH_SH`, and `PATH_CODE` correctly point
   to the respective binaries.

1. Build the project:

        cd vscode-emacs-wrapper
        gcc code.c -o code

1. Install the binary:
   1. Replace VSCode binary with the one built above.
      - Remember to back up the current VSCode binary.

   1. For an alternative cleaner solution, you can place the built `code` binary in a directory like
      `~/bin` and add it to your `PATH` to take precedence over the system-installed VSCode's `code`
      binary.

### Usage

If all went well, you shouldn't have to do anything special since attempting to edit a component in
Unity, should now cause it to open inside Emacs.

## Contributing

Contributions are what make the open-source community such an amazing place to learn, inspire, and
create. All contributions are greatly appreciated.

## License

Distributed under the MIT License. See LICENSE for more information.
