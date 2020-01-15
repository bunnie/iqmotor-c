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

#include <stdint.h>
#include <stddef.h>
#include "bipbuffer.h"

    ///
    /// \brief Clears the buffer of any allocations.
    ///
    /// Clears the buffer of any allocations or reservations. Note; it
    /// does not wipe the buffer memory; it merely resets all pointers,
    /// returning the buffer to a completely empty state ready for new
    /// allocations.
    ///
    static void Clear(struct BipBuffer *self)
    {
        self->ixa = self->sza = self->ixb = self->szb = self->ixResrv = self->szResrv = 0;
    }

    // Reserve
    //
    // Reserves space in the buffer for a memory write operation
    //
    // Parameters:
    //   int size                amount of space to reserve
    //   uint16_t& reserved        size of space actually reserved
    //
    // Returns:
    //   uint8_t*                    pointer to the reserved block
    //
    // Notes:
    //   Will return NULL for the pointer if no space can be allocated.
    //   Can return any value from 1 to size in reserved.
    //   Will return NULL if a previous reservation has not been committed.
    static uint8_t* Reserve(struct BipBuffer *self, uint16_t size, uint16_t *reserved)
    {
        // We always allocate on B if B exists; this means we have two blocks and our buffer is filling.
        if (self->szb)
        {
            uint16_t freespace = self->GetBFreeSpace(self);

            if (size < freespace) freespace = size;

            if (freespace == 0) return NULL;

            self->szResrv = freespace;
            *reserved = freespace;
            self->ixResrv = self->ixb + self->szb;
            return self->pBuffer + self->ixResrv;
        }
        else
        {
            // Block b does not exist, so we can check if the space AFTER a is bigger than the space
            // before A, and allocate the bigger one.
            uint16_t freespace = self->GetSpaceAfterA(self);
            if (freespace >= self->ixa) // If space after A > space before
            {
                if (freespace == 0) return NULL;
                if (size < freespace) freespace = size;

                self->szResrv = freespace;
                *reserved = freespace;
                self->ixResrv = self->ixa + self->sza;
                return self->pBuffer + self->ixResrv;
            }
            else // space before A > space after A
            {
                if (self->ixa == 0) return NULL;
                if (self->ixa < size) size = self->ixa;
                self->szResrv = size;
                *reserved = size;
                self->ixResrv = 0;
                return self->pBuffer;
            }
        }
    }

    // Commit
    //
    // Commits space that has been written to in the buffer
    //
    // Parameters:
    //   uint16_t size                number of bytes to commit
    //
    // Notes:
    //   Committing a size > than the reserved size will cause an assert in a debug build;
    //   in a release build, the actual reserved size will be used.
    //   Committing a size < than the reserved size will commit that amount of data, and release
    //   the rest of the space.
    //   Committing a size of 0 will release the reservation.
    static void Commit(struct BipBuffer *self, uint16_t size)
    {
      if (size == 0)
      {
        // decommit any reservation
        self->szResrv = self->ixResrv = 0;
        return;
      }

      self->CommitPartial(self, size);
      
      // Decommit rest of reservation
      self->ixResrv = 0;
      self->szResrv = 0;
    }

    // CommitPartial
    //
    // Commits space that has been written to in the buffer, but does not cancel the rest of the reservation
    //
    // Parameters:
    //   uint16_t size                number of bytes to commit
    //
    // Returns:
    //   uint16_t                     number of bytes actually committed
    //
    // Notes:
    //   Committing a size > than the reserved size will cause an assert in a debug build;
    //   in a release build, the actual reserved size will be used.
    //   Committing a size < than the reserved size will commit that amount of data, but will not realese the remaining reservation
    //   Committing a size of 0 does nothing (space is still reserved)
    static uint16_t CommitPartial(struct BipBuffer *self, uint16_t size)
    {
      // If we try to commit more space than we asked for, clip to the size we asked for.
      if (size > self->szResrv)
      {
        size = self->szResrv;
      }

      // If we have no blocks being used currently, we create one in A.
      if (self->sza == 0 && self->szb == 0)
      {
        self->ixa = self->ixResrv;
      }

      // If the reserve index is at the end of block A
      if (self->ixResrv == self->sza + self->ixa)
      {
        self->sza += size; // Grow A by committed size
      }
      else
      {
        self->szb += size; // Otherwise grow B by committed size
      }
      
      // Advance the reserved index
      self->ixResrv += size;
      // Update reserved size
      self->szResrv -= size;
      
      return size;
    }
    
    // GetContiguousBlock
    //
    // Gets a pointer to the first contiguous block in the buffer, and returns the size of that block.
    //
    // Parameters:
    //   uint16_t & size            returns the size of the first contiguous block
    //
    // Returns:
    //   uint8_t*                    pointer to the first contiguous block, or NULL if empty.
    static uint8_t* GetContiguousBlock(struct BipBuffer *self, uint16_t *size)
    {
        if (self->sza == 0)
        {
            *size = 0;
            return NULL;
        }

        *size = self->sza;
        return self->pBuffer + self->ixa;

    }

    // DecommitBlock
    //
    // Decommits space from the first contiguous block
    //
    // Parameters:
    //   int size                amount of memory to decommit
    //
    // Returns:
    //   nothing
    static void DecommitBlock(struct BipBuffer *self, uint16_t size) 
    {
        if (size >= self->sza)
        {
            self->ixa = self->ixb;
            self->sza = self->szb;
            self->ixb = 0;
            self->szb = 0;
        }
        else
        {
            self->sza -= size;
            self->ixa += size;
        }
    }

    // GetCommittedSize
    //
    // Queries how much data (in total) has been committed in the buffer
    //
    // Parameters:
    //   none
    //
    // Returns:
    //   uint16_t         total amount of committed data in the buffer
    static uint16_t GetCommittedSize(struct BipBuffer *self)
    {
        return self->sza + self->szb;
    }

    // GetReservationSize
    //
    // Queries how much space has been reserved in the buffer.
    //
    // Parameters:
    //   none
    //
    // Returns:
    //   uint16_t                    number of bytes that have been reserved
    //
    // Notes:
    //   A return value of 0 indicates that no space has been reserved
    static uint16_t GetReservationSize(struct BipBuffer *self)
    {
        return self->szResrv;
    }

    // GetBufferSize
    //
    // Queries the maximum total size of the buffer
    //
    // Parameters:
    //   none
    //
    // Returns:
    //   uint16_t                    total size of buffer
    static uint16_t GetBufferSize(struct BipBuffer *self)
    {
        return self->buflen;
    }

    // IsInitialized
    //
    // Queries whether or not the buffer has been allocated
    //
    // Parameters:
    //   none
    //
    // Returns:
    //   uint8_t                    true if the buffer has been allocated
    static uint8_t IsInitialized(struct BipBuffer *self)
    {
        return self->pBuffer != NULL;
    }

    static uint16_t GetSpaceAfterA(struct BipBuffer *self)
    {
        return self->buflen - self->ixa - self->sza;
    }

    static uint16_t GetBFreeSpace(struct BipBuffer *self)
    {
        return self->ixa - self->ixb - self->szb;
    }

void BipBuffer_init(struct BipBuffer *self, uint8_t *buffer_in, uint16_t buffer_length_in) {
  self->pBuffer = buffer_in;
  self->ixa = 0;
  self->sza = 0;
  self->ixb = 0;
  self->szb = 0;
  self->buflen = buffer_length_in;
  self->ixResrv = 0;
  self->szResrv = 0;
  self->Reserve = Reserve;
  self->Clear = Clear;
  self->Commit = Commit;
  self->CommitPartial = CommitPartial;
  self->GetContiguousBlock = GetContiguousBlock;
  self->DecommitBlock = DecommitBlock;
  self->GetCommittedSize = GetCommittedSize;
  self->GetReservationSize = GetReservationSize;
  self->GetBufferSize = GetBufferSize;
  self->IsInitialized = IsInitialized;
  self->GetSpaceAfterA = GetSpaceAfterA;
  self->GetBFreeSpace = GetBFreeSpace;
}

