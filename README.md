# quickfuck
Quickfuck is an optimizing brainfuck interpreter. It does however not yet support more than the standard brainfuck dialect.

## How to Compile

If you are using MacOS or Linux, just open the command line, move to the 'src'-directory and run 'make'.
This should create an executable file called 'qf'.

If you are using Windows and have g++ installed, open cmd.exe and move to the 'src'-directory.
Now run this command:

<tt>g++ \*.cpp -O3 -o qf.exe</tt>

Now there should be an executable file called 'qf.exe'

## Usage

Run the program in the command line. You can specify a file as an argument which is then executed.
Alternatively you can run the program and enter the code then. To start the execution, submit the code
with an EOF-character (Ctrl+D on most unix-like systems, Ctrl+Z on Windows).

## Details

There are a few things important to note when pushing it to the limit:

The memory is not guaranteed to be aligned in a circular way, i.e. when executing the following program
<tt>+[>+]</tt> it will probably exit with a segmentation fault. If you need the memory to be aligned in a circular way,
run quickfuck with argument '-i' which will run quickfuck in interpret-only mode.
However, running quickfuck in this mode is generally a lot slower than otherwise, which is why it is normally better to avoid it.
