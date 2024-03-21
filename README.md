# wine_injector

## usage
1. build the project first (see building)
2. run it:
    ```
    WINEDEBUG=-all wine wine_injector.exe <path/to/dll> <path/to/exe> <optional args for exe>
    ```
3. wait for the process to fully load and then press enter in the terminal

## building

```
$ git clone https://github.com/ten0s/wine_injector
$ cd wine_injector
$ make
```
