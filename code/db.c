#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;


#define Kibibytes(Value) ((Value) * 1024LL)
#define Mebibytes(Value) (Kibibytes(Value) * 1024LL)
#define Gibibytes(Value) (Mebibytes(Value) * 1024LL)
#define Pi32 3.14159265359f
#define internal static
#define global_variable static
#define local_persist static
#define Assert(Expression)			\
  if(!(Expression)) {*(int*)0 = 0;}
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#include "db.h"

internal void
ReadInput(input_buffer* Buffer)
{
  ssize_t BytesRead = getline((char**)&Buffer->Memory, &Buffer->BufferLen, stdin);
  if(BytesRead <= 0)
    {
      printf("ERROR::COULD NOT READ INPUT STREAM\n");
      exit(1);
    }
  // NOTE(l4v): Ignoring the delimiter
  Buffer->InputLen = BytesRead - 1;
  ((char*)Buffer->Memory)[Buffer->InputLen] = 0;
}

internal meta_command_result
ExecuteMetaCommand(input_buffer* Buffer)
{
  if(strcmp((char*)Buffer->Memory, ".exit") == 0)
    {
      return META_COMMAND_EXIT;
    }
  else
    {
      return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

internal void
ExecuteStatement(statement* Statement)
{
  switch(Statement->Type)
    {
    case STATEMENT_INSERT:
      {
	// TODO(l4v): Implement INSERT
      }break;
    case STATEMENT_SELECT:
      {
	// TODO(l4v): Implement SELECT
      }break;
    }
}


internal prepare_result
PrepareStatement(statement* Statement , input_buffer* Buffer)
{
  prepare_result Result = PREPARE_UNRECOGNIZED_STATEMENT;

  if(strncmp((char*)Buffer->Memory, "INSERT", 6) == 0)
  {
    Statement->Type = STATEMENT_INSERT;
    Result = PREPARE_SUCCESS;
    return Result;
  }

  if(strcmp((char*)Buffer->Memory, "SELECT") == 0)
    {
      Statement->Type = STATEMENT_SELECT;
      Result = PREPARE_SUCCESS;
      return Result;
    }
  
  return Result;
}

int main()
{
  // NOTE(l4v): Allocating one main memory block at start of program
  program_memory MainMemory = {};
  MainMemory.TableMemorySize = Mebibytes(4);
  MainMemory.TransientMemorySize = Kibibytes(64);
  size_t MemorySize = MainMemory.TableMemorySize + MainMemory.TransientMemorySize;
  MainMemory.TableMemory = mmap(MainMemory.TableMemory,
			   MemorySize,
			   PROT_READ | PROT_WRITE,
			   MAP_ANONYMOUS | MAP_PRIVATE,
			   -1,
			   0);
  MainMemory.TransientMemory = ((uint8*)MainMemory.TableMemory + MainMemory.TableMemorySize);
  
  input_buffer* InputBuffer = (input_buffer*)MainMemory.TransientMemory;
  *InputBuffer = (input_buffer){};
  // NOTE(l4v): REPL
  bool32 Running = 1;
  while(Running)
    {
      printf("db > ");
      ReadInput(InputBuffer);
      
      // NOTE(l4v): Check for meta commands
      if(((char*)InputBuffer->Memory)[0] == '.')
	{
	  switch(ExecuteMetaCommand(InputBuffer))
	    {
	    case META_COMMAND_EXIT:
	      {
		printf("Exiting...\n");
		Running = 0;
		continue;
	      }break;
	      
	    case META_COMMAND_SUCCESS:
	      {
		continue;
	      }break;

	    case META_COMMAND_UNRECOGNIZED_COMMAND:
	      {
		printf("Unrecognized meta command\n");
		continue;
	      }break;
	    }
	}

      // NOTE(l4v): Statement handling
      statement Statement = {};
      switch(PrepareStatement(&Statement, InputBuffer))
	{
	case PREPARE_SUCCESS:
	  {
	    // TODO(l4v): Handle statement
	  }break;

	case PREPARE_UNRECOGNIZED_STATEMENT:
	  {
	    printf("Unrecognized keyword at start of: %s\n", (char*)InputBuffer->Memory);
	    continue;
	  }break;
	  
	}

      ExecuteStatement(&Statement);
      printf("Executed\n");
    }

  munmap(MainMemory.TableMemory, MemorySize);
  return 0;
}
