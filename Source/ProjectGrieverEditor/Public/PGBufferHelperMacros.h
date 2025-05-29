#pragma once

#define READ_VAR_DECLARED(type, Name, Buffer) Name = *reinterpret_cast<const type*>(Buffer); Buffer += sizeof(type)
#define READ_VAR(type, Name, Buffer) type Name; READ_VAR_DECLARED(type, Name, Buffer)
#define READ_VAR_CONST(type, Name, Buffer) const type Name = *reinterpret_cast<const type*>(Buffer); Buffer += sizeof(type)

#define DATA_SEEK(Buffer, offset) Buffer += offset*sizeof(uint8)
