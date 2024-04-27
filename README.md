## UID: 206285524

# Pipe Up

A simple C program that simulates the Unix pipe (`|`) functionality to pass output from one process to another. 

## Building

To build the program, navigate to the directory containing the source code and type the following command:

```shell
make
```

This will compile the `pipe.c` file and create an executable named `pipe`.

## Running

To run the program, use the following command:

```shell
./pipe [command1] [command2] [command3] ...
```

Here is an example of running the program with three commands:

```shell
./pipe ls wc cat
```

This will execute the `ls` command to list directory contents, pass its output to `wc` which will count the lines, words, and characters, and then pass `wc`'s output to `cat`, which will display it. Expect to see the output of `ls` being processed by `wc` and then displayed by `cat`.

## Error Handling

The `pipe` program includes robust error handling to ensure stability and provide clear feedback in case of issues:

### Examples of Error Handling

1. **No argument**

   **Input:**

   ```shell
   ./pipe
   ```

   **Output:**

   ```shell
   Usage: ./pipe [program1] [program2]...
   ```
   This example demonstrates what happens if no argument is specified. The program outputs an error message indicating the correct usage of this program.

2. **Wrong argument**

   **Input:**

   ```shell
   ./pipe ls foo
   ```

   **Output:**

   ```shell
   Wrong argument: No such file or directory
   ```

   This example demonstrates what happens if a non-existent command (`foo`) is specified. The program outputs an error message indicating that the command could not be found.

3. **File Descriptor Redirection Failures**

   **Input:**

   ```shell
   ./pipe command_with_fd_close ls wc
   ```

   **Output:**

   ```shell
   Failed to set up stdout: Bad file descriptor
   ```

   This example shows what happens if a command within the pipeline explicitly closes its file descriptor before it can be duplicated. The error message explains that the standard output could not be redirected properly.

## Cleaning up

To clean up all binary files (executables and object files), run the following command in the project directory:

```shell
make clean
```

This will remove all compiled files, leaving only the source code files.

## Testing

```python
python -m unittest
```

### Expected Results

The test cases should pass without any errors, indicating the program is correctly running. After testing, the keyword **OK** will be printed which represents the success.
