// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 Fredrik Noring
 */

#ifndef PSGPLAY_STEREO_H
#define PSGPLAY_STEREO_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

struct psgplay;		/* PSG play object */
struct psgplay_digital;	/* PSG play digital sample */

/**
 * struct psgplay_stereo - PSG play stereo sample
 * @left: 16-bit left sample
 * @right: 16-bit right sample
 */
struct psgplay_stereo {
	int16_t left;
	int16_t right;
};

/**
 * psgplay_read_stereo - read PSG play stereo samples
 * @pp: PSG play object
 * @buffer: buffer to read into, can be %NULL to ignore
 * @count: number of stereo (left and right) sample pairs to read
 *
 * Return: number of read stereo sample pairs, zero for end of samples
 * indicating PSG play has been stopped, or negative on failure
 */
ssize_t psgplay_read_stereo(struct psgplay *pp,
	struct psgplay_stereo *buffer, size_t count, ssize_t *d_refreshed);

/**
 * psgplay_digital_to_stereo_cb - callback type to transform digital samples
 * 	into stereo samples
 * @pp: PSG play object
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: argument supplied to psgplay_digital_to_stereo_callback()
 */
typedef void (*psgplay_digital_to_stereo_cb)(struct psgplay *pp,
	struct psgplay_stereo *stereo, const struct psgplay_digital *digital,
	size_t count, void *arg);

/**
 * psgplay_digital_to_stereo_empiric - empiric stereo mix of digital samples
 * @pp: PSG play object
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: ignored, can be %NULL
 *
 * YM2149 PSG channel mix, as measured on Atari ST hardware.
 */
void psgplay_digital_to_stereo_empiric(struct psgplay *pp,
	struct psgplay_stereo *stereo, const struct psgplay_digital *digital,
	size_t count, void *arg);

/**
 * psgplay_digital_to_stereo_linear - linear stereo mix of digital samples
 * @pp: PSG play object
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: ignored, can be %NULL
 */
void psgplay_digital_to_stereo_linear(struct psgplay *pp,
	struct psgplay_stereo *stereo, const struct psgplay_digital *digital,
	size_t count, void *arg);

/**
 * struct psgplay_psg_stereo_balance - PSG play stereo channel balance
 * @a: channel A balance, range -1 (left) .. 0 (mono) .. +1 (right)
 * @b: channel B balance, range -1 (left) .. 0 (mono) .. +1 (right)
 * @c: channel C balance, range -1 (left) .. 0 (mono) .. +1 (right)
 */
struct psgplay_psg_stereo_balance {
	float a;
	float b;
	float c;
};

/**
 * psgplay_digital_to_stereo_balance - balance stereo mix of digital samples
 * @pp: PSG play object
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: pointer to struct psgplay_psg_stereo_balance
 */
void psgplay_digital_to_stereo_balance(struct psgplay *pp,
	struct psgplay_stereo *stereo, const struct psgplay_digital *digital,
	size_t count, void *arg);

/**
 * struct psgplay_psg_stereo_volume - PSG play stereo channel volume
 * @a: channel A volume, range 0 (off) .. +1 (max)
 * @b: channel B volume, range 0 (off) .. +1 (max)
 * @c: channel C volume, range 0 (off) .. +1 (max)
 */
struct psgplay_psg_stereo_volume {
	float a;
	float b;
	float c;
};

/**
 * psgplay_digital_to_stereo_volume - volume stereo mix of digital samples
 * @pp: PSG play object
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: pointer to struct psgplay_psg_stereo_volume
 */
void psgplay_digital_to_stereo_volume(struct psgplay *pp,
	struct psgplay_stereo *stereo, const struct psgplay_digital *digital,
	size_t count, void *arg);

/**
 * psgplay_digital_to_stereo_callback - invoke callback to transform digital
 * 	samples into stereo samples
 * @pp: PSG play object
 * @cb: callback
 * @arg: optional argument supplied to @cb, can be %NULL
 */
void psgplay_digital_to_stereo_callback(struct psgplay *pp,
	const psgplay_digital_to_stereo_cb cb, void *arg);

/**
 * psgplay_stereo_downsample_cb - callback type to downsample stereo samples
 * @resample: downsampled stereo samples
 * @stereo: 250.332 kHz stereo samples
 * @count: number of stereo samples to downsample
 * @arg: argument supplied to psgplay_stereo_downsample_callback()
 *
 * Return: number of resamples, must be equal or less than @count
 */
typedef size_t (*psgplay_stereo_downsample_cb)(
	struct psgplay_stereo *resample, const struct psgplay_stereo *stereo,
	size_t count, void *arg);

/**
 * psgplay_stereo_downsample_callback - invoke callback to downsample
 * 	stereo samples
 * @pp: PSG play object
 * @cb: callback
 * @arg: optional argument supplied to @cb, can be %NULL
 */
void psgplay_stereo_downsample_callback(struct psgplay *pp,
	const psgplay_stereo_downsample_cb cb, void *arg);

/**
 * struct psgplay_psg_stereo_empiric_lpf - PSG play stereo empiric with Low-Pass Filter
 * @a0: coeffecient applied to input sample, filled in by psgplay_calculate_empiric_lpf()
 * @a1: coeffecient applied to historic input sample, filled in by psgplay_calculate_empiric_lpf()
 * @a2: coeffecient applied to historic input sample, filled in by psgplay_calculate_empiric_lpf()
 * @b1: coeffecient applied to historic output sample, filled in by psgplay_calculate_empiric_lpf()
 * @b2: coeffecient applied to historic output sample, filled in by psgplay_calculate_empiric_lpf()
 * @x1: writable variable used by psgplay_digital_to_stereo_empiric_lpf(), historic input sample
 * @x2: writable variable used by psgplay_digital_to_stereo_empiric_lpf(), historic input sample
 * @y1: writable variable used by psgplay_digital_to_stereo_empiric_lpf(), historic output sample
 * @y2: writable variable used by psgplay_digital_to_stereo_empiric_lpf(), historic output sample
 */
struct psgplay_psg_stereo_empiric_lpf
{
	float a0, a1, a2; /* coeffecients input */
	float     b1, b2; /* coeffecients outputs (historic only) */
	float     x1, x2; /* historic input sample */
	float     y1, y2; /* historic result sample */
};

/**
 * psgplay_calculate_empiric_lpf: calculates coeffecients to be used by psgplay_digital_to_stereo_empiric_lpf()
 * @p: target data area
 * @fc: cut-off frequency for low-pass filter, typical default value is 880
 * @fs: output sample-rate
 * @Q: Q-factor, sharpness of the cut-off frequency, must be bigger than zero, typical default value is 1.0
 */
void psgplay_calculate_empiric_lpf(struct psgplay_psg_stereo_empiric_lpf *p, const int fc, const int fs, const float Q);

/**
 * psgplay_digital_to_stereo_empiric_lpf - empiric stereo mix of digital samples, including a Low-Pass Filter on the PSG chip output.
 * @pp: PSG play object
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: pointer to a struct psgplay_psg_stereo_empiric_lpf
 *
 * Performs the same as psgplay_digital_to_stereo_empiric(), but includes a
 * Low-Pass Filter on the PSG output. Use psgplay_calculate_empiric_lpf to
 * initialize a struct psgplay_psg_stereo_empiric_lpf, which MUST to be writable
 * and preserved between calls to this function. Heavily inspired on filters
 * found on https://github.com/dimtass/dsp-c-filters
 */
void psgplay_digital_to_stereo_empiric_lpf(struct psgplay *pp,
	struct psgplay_stereo *stereo, const struct psgplay_digital *digital,
	size_t count, void *arg);

#endif /* PSGPLAY_STEREO_H */
