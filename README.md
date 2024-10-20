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

    ----- llmc params -----

    -h,    --help, --usage                  Print llmc usage
    --setup                                 Set up your llmc: choose or customize the model
    --show-config                           Show llmc configuration
    --no-explanations                       Disable explanations
    --trace                                 Enable tracing
    --model-help, --model-usage             Print detailed model usage

Supported Models
----------------

- Llama 3.2 (3B, 8bit)
- Custom models: You can provide your own model and system prompt.


Touble Shoting
----------------

ggml_metal_graph_compute: command buffer 1 failed with status 5
error: Insufficient Memory (00000008:kIOGPUCommandBufferCallbackErrorOutOfMemory)

TODO
----

- CI/CD pipeline
- multiline