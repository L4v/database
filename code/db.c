#include <stdio.h>
#include <stddef.h>
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
#define SizeOfAttribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define TABLE_MAX_PAGES 100
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

#include "db.h"

/* NOTE(l4v):
   TEMP HARDCODED TABLE

   column        type           offset(bytes)

   id            integer(4)     0
   username      varchar(32)    4
   email         varchar(255)   32+4

   total size: 291
*/
const global_variable uint32 ROW_SIZE = sizeof(row);
const global_variable uint32 PAGE_SIZE = Kibibytes(4);
const global_variable uint32 ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const global_variable uint32 TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

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

internal prepare_result
PrepareStatement(statement* Statement , input_buffer* Buffer)
{
  // NOTE(l4v): Example of INSERT statement
  // INSERT 1 John foo@bar.com
  prepare_result Result = PREPARE_UNRECOGNIZED_STATEMENT;

  if(strncmp((char*)Buffer->Memory, "INSERT", 6) == 0)
  {
    Statement->Type = STATEMENT_INSERT;
    int32 NumberOfArgs = sscanf((char*)Buffer->Memory, "INSERT %d %s %s",
				&(Statement->RowToInsert.Id),
				Statement->RowToInsert.Username,
				Statement->RowToInsert.Email);
    Result = PREPARE_SUCCESS;
    if(NumberOfArgs < 3)
      {
	Result = PREPARE_SYNTAX_ERROR;
      }
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

internal void*
GetRowSlot(table* Table, uint32 RowNum)
{
  void* Result = 0;
  /* uint32 PageNum = RowNum / ROWS_PER_PAGE; */
  /* void* Page = Table->Pages[PageNum]; */
  /* Assert(Page); */
  /* uint32 RowOffset = RowNum % ROWS_PER_PAGE; */
  /* uint32 ByteOffset = RowOffset * sizeof(row); */
  /* Result = Page + ByteOffset; */
  Result = (uint8*)Table->Pages + RowNum * sizeof(row);
  Assert(Result);
  return Result;
}

internal execute_result
ExecuteSelect(statement* Statement, table* Table)
{
  execute_result Result = EXECUTE_SUCCESS;
  row* Row;
  for(size_t RowIndex = 0;
      RowIndex < Table->NumOfRows;
      ++RowIndex)
    {
      Row = (row*)GetRowSlot(Table, RowIndex);
      printf("%d %s %s\n",
  	     Row->Id,
  	     Row->Username,
  	     Row->Email);
    }

  return Result;
}

internal execute_result
ExecuteInsert(statement* Statement, table* Table)
{
  execute_result Result = EXECUTE_SUCCESS;
  if(Table->NumOfRows >= TABLE_MAX_ROWS)
    {
      Result = EXECUTE_TABLE_FULL;
      return Result;
    }

  row* WhereToInsert = (row*)GetRowSlot(Table, Table->NumOfRows);
  *WhereToInsert = Statement->RowToInsert;
  ++(Table->NumOfRows);
  return Result;
}

internal execute_result
ExecuteStatement(statement* Statement, table* Table)
{
  execute_result Result;
  switch(Statement->Type)
    {
    case STATEMENT_INSERT:
      {
	Result = ExecuteInsert(Statement, Table);
      }break;
    case STATEMENT_SELECT:
      {
	Result = ExecuteSelect(Statement, Table);
      }break;
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

  table* Table = (table*)MainMemory.TableMemory;
  *Table = (table){};
  // NOTE(l4v): Setting up page pointers, since the memory is allocated only once, the pointers
  // are also set only once 
  // TODO(l4v): Make this neater?
  size_t PageOffset = offsetof(table, Pages);
  /* for(size_t PageIndex = 0; */
  /*     PageIndex < TABLE_MAX_PAGES; */
  /*     ++PageIndex) */
  /*   { */
  /*     Table->Pages[PageIndex] = (uint8 *)Table + PageOffset; */
  /*     PageOffset += PAGE_SIZE; */
  /*   } */

  // IMPORTANT TODO(l4v): ALLOCATION
  /* Table->Pages2 = malloc(4 * sizeof(row)); */
  Table->Pages = (uint8*)Table + PageOffset;
  
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

	case PREPARE_SYNTAX_ERROR:
	  {
	    printf("Syntax error, could not parse statement\n");
	    continue;
	  }break;
	  
	case PREPARE_UNRECOGNIZED_STATEMENT:
	  {
	    printf("Unrecognized keyword at start of: %s\n", (char*)InputBuffer->Memory);
	    continue;
	  }break;
	  
	}

      switch(ExecuteStatement(&Statement, Table))
	{
	case EXECUTE_SUCCESS:
	  {
	    printf("Executed\n");
	  }break;

	case EXECUTE_TABLE_FULL:
	  {
	    printf("Error: Table is full\n");
	  }break;
	}
    }

  munmap(MainMemory.TableMemory, MemorySize);
  return 0;
}
