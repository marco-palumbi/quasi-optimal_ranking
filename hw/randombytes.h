// =============================================
// ==    Classification: TII CONFIDENTIAL     ==
// =============================================

#ifndef RANDOMBYTES_H
#define RANDOMBYTES_H

/**
 * @file randombytes.h
 *
 * @brief Interface to the RNG functions.
 *
 * @details pqclib-lw needs a high-quality RNG to work. The implementation of this is left to the user who must implement the
 *          `randombytes()` function possibly using suitable hardware on the target system.
 *          The masked versions needs an additional source of randmones, the `random_x_masking()` function.
 *          Also this must be provided by the user.
 *
 */
#include <stddef.h>
#include <stdint.h>

#define RANDOMBYTES_SUCCESS 0    /**< Exit code in case of success */
#define RANDOMBYTES_FAILURE (-1) /**< Exit code in case of failure */

/**
 * @brief Get high quality randomness from the system.
 *
 * @param[out] buf  pointer to output nuffer to be filled with random values (allocated
 *                  array of 'size' bytes)
 * @param[in]  size size of the buffer
 *
 * @return          RANDOMBYTES_SUCCESS on success
 *                  RANDOMBYTES_FAILURE on error
 */
int randombytes(void *buf, size_t size);

/**
 * @brief Get high quality randomness for the gadgets of the masked implementation.
 *
 * @param[out] buf  pointer to output nuffer to be filled with random values (allocated
 *                  array of 'size' bytes)
 * @param[in]  size size of the buffer
 *
 * @return          RANDOMBYTES_SUCCESS on success
 *                  RANDOMBYTES_FAILURE on error
 */
int random_x_masking(void *buf, size_t size);

#endif
