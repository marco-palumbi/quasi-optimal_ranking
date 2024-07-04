# quasi-optimal_ranking
Accompanying repository for [`Quasi-Optimal Permutation Ranking and Applications to PERK`](https://ia.cr/2024/412) AFRICACRYPT 2024 paper.

This library contains the implementations in C of ranking/unranking algorithms for permutations in the following flavors.:

- The classic permutation ranking/unranking algorithms in two versions: one using [tiny-bignum-c](https://github.com/kokke/tiny-bignum-c?tab=readme-ov-file) and one [GMP](https://gmplib.org/) for arbitrary-arithmetic precision.
- The [Bonet variant](https://bonetblai.github.io/reports/AAAI08-ws10-ranking.pdf) permutation ranking/unranking algorithms also implemented with [tiny-bignum-c](https://github.com/kokke/tiny-bignum-c?tab=readme-ov-file) and [GMP](https://gmplib.org/).
- The [quasi-optimal permutation ranking/unranking](https://ia.cr/2024/412) algorithms which do not require arbitrary-arithmetic precision. In addition, we provide AVX/AVX2 optimizations for certain parameters in PERK v1.1.

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
