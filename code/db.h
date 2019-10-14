typedef enum
  {
   META_COMMAND_SUCCESS = 0,
   META_COMMAND_EXIT,
   META_COMMAND_UNRECOGNIZED_COMMAND
  } meta_command_result;

typedef enum
  {
   PREPARE_SUCCESS = 0,
   PREPARE_UNRECOGNIZED_STATEMENT
  } prepare_result;

typedef enum
  {
   STATEMENT_INSERT = 0,
   STATEMENT_SELECT
  } statement_type;

typedef struct
{
  void* Memory;
  size_t BufferLen;
  ssize_t InputLen;
} input_buffer;

typedef struct
{
  void* TableMemory;
  size_t TableMemorySize;
  
  void* TransientMemory;
  size_t TransientMemorySize;
} program_memory;

typedef struct
{
  statement_type Type; 
} statement;
