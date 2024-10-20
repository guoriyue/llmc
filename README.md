llmc
====

llmc is a local inference command-line tool based on llama.cpp that helps generate shell commands from natural language descriptions.

Install
-------

Build from source:

    make llmc

Usage
-----

To use llmc, provide a prompt describing the shell command:

    llmc "your prompt"

Or, you can run `./llmc` to see available options:

--------------------------------- llmc params ----------------------------------
-h,    --help, --usage                  Print llmc usage
--setup                                 Set up your llmc model: choose or customize
--show-args                             Show arguments you saved
--save-args                             Save arguments for frequent future use
--no-explanation                        Disable command explanation
--no-edit                               Disable editing the chosen command
--mode {loop,exit}                      Select the mode of operation.
                                                - loop: Continues to choose and execute commands indefinitely.
                                                - exit: Executes a single command and then stops the program.
--model-help, --model-usage             Print llmc default model arguments
--trace                                 Enable tracing for the execution of the default model

Supported Models
----------------

- Llama 3.2 (3B, 8bit)
- Custom models: You can provide your own model and system prompt.


Touble Shoting
----------------

ggml_metal_graph_compute: command buffer 1 failed with status 5
error: Insufficient Memory (00000008:kIOGPUCommandBufferCallbackErrorOutOfMemory)