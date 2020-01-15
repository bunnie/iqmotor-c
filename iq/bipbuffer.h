/*
   Copyright (c) 2003 Simon Cooke, All Rights Reserved

   Licensed royalty-free for commercial and non-commercial
   use, without warranty or guarantee of suitability for any purpose.
   All that I ask is that you send me an email
   telling me that you're using my code. It'll make me
   feel warm and fuzzy inside. spectecjr@gmail.com

*/

/*
  Name: bibuffer.h
  Last update: 3/7/2019 by Raphael Van Hoffelen
  Author: Simon Cooke
  Contributors: Matthew Piccoli, Raphael Van Hoffelen
*/

/* 
  Changed header gaurd from pragma once to ifndef BipBuffer_H.
  Chaning types to fixed width integers.
  Removed destructor, AllocateBuffer, FreeBuffer.
  Constructor expects array and size to be passed in.
  Including stddef for NULL
  Matthew Piccoli, 4/1/2016
*/

#ifndef BipBuffer_H
#define BipBuffer_H

#include <stddef.h>

typedef struct BipBuffer
{
  uint8_t* pBuffer; // Pointer to the data buffer
  uint16_t ixa; // Starting index of region A
  uint16_t sza; // Size of region A
  uint16_t ixb; // Starting index of region B
  uint16_t szb; // Size of region B
  uint16_t buflen; // Length of full buffer
  uint16_t ixResrv; // Starting index of reserved region
  uint16_t szResrv; // Size of the reserved region
  void(*Clear)(struct BipBuffer *self);
  uint8_t *(* Reserve)( struct BipBuffer *self, uint16_t size, uint16_t *reserved);
  void(*Commit)(struct BipBuffer *self, uint16_t size);
  uint16_t(*CommitPartial)(struct BipBuffer *self, uint16_t size);
  uint8_t *(*GetContiguousBlock)(struct BipBuffer *self, uint16_t *size);
  void(*DecommitBlock)(struct BipBuffer *self, uint16_t size);
  uint16_t(* GetCommittedSize)(struct BipBuffer *self);
  uint16_t(* GetReservationSize)(struct BipBuffer *self);
  uint16_t(* GetBufferSize)(struct BipBuffer *self);
  uint8_t(* IsInitialized)(struct BipBuffer *self);
  uint16_t(* GetSpaceAfterA)(struct BipBuffer *self);
  uint16_t(* GetBFreeSpace)(struct BipBuffer *self);
} BipBuffer;

void BipBuffer_init(struct BipBuffer *self, uint8_t *buffer_in, uint16_t buffer_length_in);

#endif // BipBuffer_H
