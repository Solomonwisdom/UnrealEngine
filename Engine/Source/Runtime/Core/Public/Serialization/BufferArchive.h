// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Archive.h"
#include "MemoryWriter.h"

/**
 * Buffer archiver.
 */
class FBufferArchive : public FMemoryWriter, public TArray<uint8>
{
public:
	FBufferArchive( bool bIsPersistent = false, const FName InArchiveName = NAME_None )
	: FMemoryWriter( (TArray<uint8>&)*this, bIsPersistent, false, InArchiveName )
	{}
	/**
  	 * Returns the name of the Archive.  Useful for getting the name of the package a struct or object
	 * is in when a loading error occurs.
	 *
	 * This is overridden for the specific Archive Types
	 **/
	virtual FString GetArchiveName() const { return *FString::Printf( TEXT("FBufferArchive %s"), *ArchiveName.ToString()); }
};

