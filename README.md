# Lighthouse Problem

## Compile and Run
### Windows

- Requirements: cmake, MSVC

- Compile: 在src文件夹下：

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

- Run:
  - 运行完整项目

    ```bash
    .\Release\lighthouse.exe
    ```

    打开 http://127.0.0.1:8080/index.html，进行交互操作

  - 生成测试用例（以点数1000为例）

    ```bash
    .\Release\generation_test.exe 1000 > in.txt
    ```

  - 对测试用例执行直方图划分

    ```
    .\Release\histogram_test.exe < in.txt
    ```

  - 对测试用例求解灯塔问题

    ```
    .\Release\solve_test.exe < in.txt
    ```

- 开发环境：Windows 10, cmake 3.20.3, MSVC 19.24.28314.0 with Windows SDK 10.0.18362.0

### MacOS

- Requirements: cmake, make, g++(clang)
- Compile: 在src文件夹下：

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
- Run:

  - 运行完整项目

    ```bash
    ./lighthouse
    ```

    打开 http://127.0.0.1:8080/index.html，进行交互操作

  - 生成测试用例（以点数1000为例）

    ```bash
    ./generation_test 1000 > in.txt
    ```

  - 对测试用例执行直方图划分

    ```
    ./histogram_test < in.txt
    ```

  - 对测试用例求解灯塔问题

    ```
    ./solve_test < in.txt
    ```

- 开发环境：MacOS 10.15.6, cmake 3.18.4, GNU make 3.81, Apple clang 12.0.0




