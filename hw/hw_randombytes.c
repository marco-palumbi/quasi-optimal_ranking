// =============================================
// ==    Classification: TII CONFIDENTIAL     ==
// =============================================

/*
The MIT License

Copyright (c) 2017 Daan Sprenkels <hello@dsprenkels.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#if defined(__MSYS__)
#define _WIN32
#endif /* defined(__MSYS__) */

#include "hal.h"
#include "randombytes.h"

#if defined(_WIN32)
/* Windows */
// clang-format off
// NOLINTNEXTLINE(llvm-include-order): Include order required by Windows
#include <windows.h>
#include <wincrypt.h> /* CryptAcquireContext, CryptGenRandom */
// clang-format on
#endif /* defined(_WIN32) */

#if defined(__linux__)
/* Linux */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

// We need SSIZE_MAX as the maximum read len from /dev/urandom
#if !defined(SSIZE_MAX)
#define SSIZE_MAX (SIZE_MAX / 2 - 1)
#endif /* defined(SSIZE_MAX) */

#endif /* defined(__linux__) */

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
/* Dragonfly, FreeBSD, NetBSD, OpenBSD (has arc4random) */
#include <sys/param.h>
#if defined(BSD)
#include <stdlib.h>
#endif
#endif

#if defined(__EMSCRIPTEN__)
#include <assert.h>
#include <emscripten.h>
#include <errno.h>
#include <stdbool.h>
#endif /* defined(__EMSCRIPTEN__) */

#if defined(STM32F4)
#include <libopencm3/stm32/rng.h>
#include <stdint.h>

// TODO Maybe we do not want to use the hardware RNG for all randomness, but instead only read a seed and then expand that using fips202.

#if defined(USE_QEMU)
#include <libopencm3/stm32/usart.h>
#include "KeccakHash.h"

static int randombytes_STM32_libopencm3(uint8_t *obuf, size_t len) {
  // NOT multithreading safe
  // suitable for testing because not multithreaded environment
  static Keccak_HashInstance ctx_x_masking;
  static int ctx_x_masking_initalized = 0;
  static const uint8_t hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

  if (!ctx_x_masking_initalized) {
    uint8_t seed[32];
    printf("Reading 32 bytes bytes from Usart to seed the PRNG....\n");
    for (size_t i = 0; i < sizeof(seed); i++) {
      seed[i] = usart_recv_blocking(USARTx);
      putchar(hex[seed[i] >> 4U]);
      putchar(hex[seed[i] & 0xFU]);
    }
    putchar('\n');
    putchar('\n');
    ctx_x_masking_initalized = 1;
    Keccak_HashInitialize_SHAKE128(&ctx_x_masking);
    Keccak_HashUpdate(&ctx_x_masking, seed, sizeof(seed) * 8);
    Keccak_HashFinal(&ctx_x_masking, NULL);
  }
  Keccak_HashSqueeze(&ctx_x_masking, obuf, len * 8);
  return RANDOMBYTES_SUCCESS;
}
#else
static int randombytes_STM32_libopencm3(uint8_t *obuf, size_t len) {
  union {
    uint8_t aschar[4];
    uint32_t asint;
  } random;

  while (len > 4) {
    random.asint = rng_get_random_blocking();
    *obuf++ = random.aschar[0];
    *obuf++ = random.aschar[1];
    *obuf++ = random.aschar[2];
    *obuf++ = random.aschar[3];
    len -= 4;
  }
  if (len > 0) {
    for (random.asint = rng_get_random_blocking(); len > 0; --len) {
      *obuf++ = random.aschar[len - 1];
    }
  }

  return RANDOMBYTES_SUCCESS;
}
#endif
#endif /* defined(STM32F4) */

#if defined(_WIN32)
static int randombytes_win32_randombytes(void *buf, const size_t n) {
  HCRYPTPROV ctx;
  BOOL tmp;

  tmp = CryptAcquireContext(&ctx, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
  if (tmp == FALSE) {
    return RANDOMBYTES_FAILURE;
  }

  tmp = CryptGenRandom(ctx, (DWORD)n, (BYTE *)buf);
  if (tmp == FALSE) {
    return RANDOMBYTES_FAILURE;
  }

  tmp = CryptReleaseContext(ctx, 0);
  if (tmp == FALSE) {
    return RANDOMBYTES_FAILURE;
  }

  return RANDOMBYTES_SUCCESS;
}
#endif /* defined(_WIN32) */

#if defined(__linux__)
static int fd = -1;
static int randombytes_linux_randombytes_urandom(void *buf, size_t n) {
  size_t offset = 0, count;
  ssize_t tmp;

  if (fd == -1) {  // open /dev/urandom only once
    do {
      fd = open("/dev/urandom", O_RDONLY);
    } while (fd == -1 && errno == EINTR);
    if (fd == -1) {
      return RANDOMBYTES_FAILURE;
    }
  }

  while (n > 0) {
    count = n <= SSIZE_MAX ? n : SSIZE_MAX;
    tmp = read(fd, (char *)buf + offset, count);
    if (tmp == -1 && (errno == EAGAIN || errno == EINTR)) {
      continue;
    }
    if (tmp == -1) {
      return RANDOMBYTES_FAILURE; /* Unrecoverable IO error */
    }
    offset += tmp;
    n -= tmp;
  }
  assert(n == 0);
  return RANDOMBYTES_SUCCESS;
}
#endif /* defined(__linux__) */

#if defined(BSD)
static int randombytes_bsd_randombytes(void *buf, size_t n) {
  arc4random_buf(buf, n);
  return RANDOMBYTES_SUCCESS;
}
#endif /* defined(BSD) */

#if defined(__EMSCRIPTEN__)
static int randombytes_js_randombytes_nodejs(void *buf, size_t n) {
  const int ret = EM_ASM_INT(
    {
      var crypto;
      try {
        crypto = require('crypto');
      } catch (error) {
        return -2;
      }
      try {
        writeArrayToMemory(crypto.randomBytes($1), $0);
        return 0;
      } catch (error) {
        return -1;
      }
    },
    buf, n);
  switch (ret) {
    case 0:
      return RANDOMBYTES_SUCCESS;
    case -1:
      errno = EINVAL;
      return RANDOMBYTES_FAILURE;
    case -2:
      errno = ENOSYS;
      return RANDOMBYTES_FAILURE;
  }
  assert(false);  // Unreachable
}
#endif /* defined(__EMSCRIPTEN__) */

int hw_randombytes(uint8_t *buf, size_t n) {
#if defined(STM32F4)
  return randombytes_STM32_libopencm3(buf, n);
#elif defined(__EMSCRIPTEN__)
  return randombytes_js_randombytes_nodejs(buf, n);
#elif defined(__linux__)
  /* When we have enough entropy, we can read from /dev/urandom */
  return randombytes_linux_randombytes_urandom(buf, n);
#elif defined(BSD)
  /* Use arc4random system call */
  return randombytes_bsd_randombytes(buf, n);
#elif defined(_WIN32)
  /* Use windows API */
  return randombytes_win32_randombytes(buf, n);
#else
#error "randombytes(...) is not supported on this platform"
#endif
}
