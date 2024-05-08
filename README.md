# quasi-optimal_ranking
Accompanying repository for `Quasi-Optimal Permutation Ranking and Applications to PERK` AFRICACRYPT paper.

Authors:

- Slim Bettaieb
- Alessandro Budroni
- Décio Luiz Gazzoni Filho 
- Marco Palumbi

## build and run on Linux x86
The avx2 code has been optimized for the **clang** compiler

### Prerequisites ###

- standard development tools: **CMake**, **make**, **git**
- **clang** compiler (instal with: `sudo apt install clang`)
- **lld** linker (instal with: `sudo apt install lld`)

### configure and build ###

```bash
cmake -DCMAKE_INSTALL_PREFIX=./cmake-build/binaries -S. -Bcmake-build -DCMAKE_BUILD_TYPE=Release -DPARAM_N1=79 -DCMAKE_C_COMPILER=clang
make -C cmake-build/
#run
./cmake-build/test_xxx
```

## build and run on [**STM32F407G-DISC1**](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) evaluation board

### Prerequisites ###

- **arm-none-eabi-gcc** [toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).
- [**stlink**](https://github.com/texane/stlink) to flash binaries into the development board. Some distribution have packages for this.
- standard development tools: **CMake**, **make**, **git**
- **python** and **python3** (both command must be present and running on the development machine).
- may be Im missing some....

### configure and build ###

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=arm-none-eabi-libopencm3.toolchain  -DCMAKE_INSTALL_PREFIX=./cmake-build/binaries -S. -Bcmake-build -DCMAKE_BUILD_TYPE=Release -DPARAM_N1=79
make -C cmake-build/
#run
st-flash --reset write  cmake-build/test_xxx  0x8000000
```

## Additional Cmake configuration flags

Call cmake with `-DPARAM_N1=NN` to build tests for N1=NN
